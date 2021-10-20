/* -*- P4_14 -*- */
#ifdef __TARGET_TOFINO__
#include <tofino/constants.p4>
#include <tofino/intrinsic_metadata.p4>
#include <tofino/primitives.p4>
//Include the blackbox definition
#include <tofino/stateful_alu_blackbox.p4>
#else
#warning This program is intended for Tofino P4 architecture only
#endif

#define BUCKETS 8192
#define BUCKETS_HASH 13
#define MICE_LEN 131072 // namely, 2**17
#define MICE_LEN_HASH 17
#define LAMBDA 5
// 3 * 2**13
/*--*--* HEADERS *--*--*/
header_type Ethernet {
    fields {
        dstAddr : 48;
        srcAddr : 48;
        etherType : 16;
    }
}

header_type Ipv4 {
    fields {
        version : 4;
        ihl : 4;
        diffserv : 8;
        totalLen : 16;
        identification : 16;
        flags : 3;
        fragOffset : 13;
        ttl : 8;
        protocol : 8;
        hdrChecksum : 16;
        srcAddr : 32;
        dstAddr : 32;
    }
}

header_type MyFlow {
    // header_type for parsing flow packets
    fields {
        id: 32;
    }
}

header_type MyMeta {
    fields {
        totVotes: 32; // field for reading the value of total votes from stateful registers
        totVotes_div: 32; // field to store the result of totVotes/LAMBDA
        flow_id: 32; // field to store id of cur inserted flow
        flow_freq: 32; // field to store freq of cur inserted flow
        register_id: 32; //field to temporarily memorize the value from stateful registers
        heavy_flag: 1; // true if insert flow to heavy
        light_flag: 1; // true if insert flow to light
    }
}

header Ethernet ethernet;
header Ipv4 ipv4;
header MyFlow myflow;
metadata MyMeta meta;


/*--*--* PARSERS *--*--*/
parser start {
    return parse_ethernet;
}

parser parse_ethernet {
    extract(ethernet);
    return parse_ipv4;
}

parser parse_ipv4 {
    extract(ipv4);
    return parse_myflow;
}

parser parse_myflow {
    extract(myflow);
    set_metadata(meta.flow_id, myflow.id);
    set_metadata(meta.flow_freq, 1);
    return ingress;
}


/*--*--* REGISTERS *--*--*/
// registers for step 1
register totVotesReg1 {
    width: 32; //64
    instance_count: BUCKETS;
}
register freqIdReg1 {
    width: 64;
    instance_count: BUCKETS;
}

// registers for step 2
register totVotesReg2 {
    width: 32; //64
    instance_count: BUCKETS;
}
register freqIdReg2 {
    width: 64;
    instance_count: BUCKETS;
}

// registers for step 3
register totVotesReg3 {
    width: 32; //64
    instance_count: BUCKETS;
}
register freqIdReg3 {
    width: 64;
    instance_count: BUCKETS;
}

// registers for step 4
register totVotesReg4 {
    width: 32; //64
    instance_count: BUCKETS;
}
register freqIdReg4 {
    width: 64;
    instance_count: BUCKETS;
}

// registers for step 5
register miceReg {
    width: 8;
    instance_count: MICE_LEN;
}

/*--*--* Hash *--*--*/
field_list hash_list {
    meta.flow_id;
}

field_list_calculation hash_heavy {
    input { hash_list; }
    algorithm : identity;
    output_width : BUCKETS_HASH;
}

field_list_calculation hash_heavy_2 {
    input { hash_list; }
    algorithm : identity;
    output_width : BUCKETS_HASH;
}

field_list_calculation hash_heavy_3 {
    input { hash_list; }
    algorithm : identity;
    output_width : BUCKETS_HASH;
}

field_list_calculation hash_heavy_4 {
    input { hash_list; }
    algorithm : identity;
    output_width : BUCKETS_HASH;
}



field_list_calculation hash_mice {
    input { hash_list; }
    algorithm : identity;
    output_width : MICE_LEN_HASH;
}


/*--*--* actions for all steps *--*--*/
action mydrop()
{
    modify_field(meta.heavy_flag, 0);
    modify_field(meta.light_flag, 0);
}

action voteDivAction() {
    shift_right(meta.totVotes_div, meta.totVotes, LAMBDA);
}

action updateAction() {
    modify_field(meta.flow_id, meta.register_id);
    modify_field(meta.flow_freq, meta.totVotes_div);
}

/*---------------------step 1---------------------*/
table dropTable1 {
    // table to wrap up real_drop action in step 1
    actions {
        mydrop;
    }
    default_action: mydrop();
}

blackbox stateful_alu totVotesSalu1 {
     // S-alu to read and manipulate total votes in step 1
     //
     // The value of vote(all) is increased by 1 whenever a flow is comming,
     // and the result is output to meta.totVotes field.

    reg: totVotesReg1;
    update_lo_1_value: register_lo + 1;
    output_value: alu_lo;
    output_dst: meta.totVotes;
}
action totVotesAction1() {
    // action to execute S-alu while initiating the meta.register_id field
    totVotesSalu1.execute_stateful_alu_from_hash(hash_heavy);
    modify_field(meta.register_id, 0);
    modify_field(meta.heavy_flag, 1);
    modify_field(meta.light_flag, 1);
}
table totVotesTable1 {
    // table to wrap up totVotesAction1
    actions {
        totVotesAction1;
    }
    default_action: totVotesAction1();
}

table voteDivTable1 {
    // table to wrap up voteDivAction1
    actions {
        voteDivAction;
    }
    default_action: voteDivAction();
}

blackbox stateful_alu freqIdSalu1 {
    // S-alu to read and manipulate the 64-bit register containing values of both vote+ and flowID

     // 2 conditions are evaluated here. If both are true, the coming flow will kick out
     // the original flow stored in the register and output the original flowID into
     // meta.register_id field. Otherwise, it will do nothing.

    reg: freqIdReg1;

    condition_hi: meta.totVotes_div >= register_hi;
    condition_lo: myflow.id == register_lo;

    update_lo_1_predicate: condition_lo or condition_hi;
    update_lo_1_value: myflow.id;

    update_hi_1_predicate: condition_lo or condition_hi;
    update_hi_1_value: register_hi + 1;

    output_predicate: condition_lo or condition_hi;
    output_value: register_lo;
    output_dst: meta.register_id;
}
action freqIdAction1() {
    // action to wrap up freqIdSalu1
    freqIdSalu1.execute_stateful_alu_from_hash(hash_heavy);
}

table freqIdTable1 {
    // table to wrap up freqIdAction1
    actions {
        freqIdAction1;
    }
    default_action: freqIdAction1();
}

table updateTable1 {
    // table to wrap up freqIdTable1
    actions {
        updateAction;
    }
    default_action: updateAction();
}

/*---------------------step 2---------------------*/

table dropTable2 {
    actions {
        mydrop;
    }
    default_action: mydrop();
}

blackbox stateful_alu totVotesSalu2 {
    reg: totVotesReg2;
    update_lo_1_value: register_lo + 1;
    output_value: alu_lo;
    output_dst: meta.totVotes;
}
action totVotesAction2() {
    totVotesSalu2.execute_stateful_alu_from_hash(hash_heavy_2);
    modify_field(meta.register_id, 0);
}
table totVotesTable2 {
    actions {
        totVotesAction2;
    }
    default_action: totVotesAction2();
}

table voteDivTable2 {
    actions {
        voteDivAction;
    }
    default_action: voteDivAction();
}

blackbox stateful_alu freqIdSalu2 {
    reg: freqIdReg2;

    condition_hi: meta.totVotes_div >= register_hi;
    condition_lo: myflow.id == register_lo;

    update_lo_1_predicate: condition_lo or condition_hi;
    update_lo_1_value: myflow.id;

    update_hi_1_predicate: condition_lo or condition_hi;
    update_hi_1_value: register_hi + 1;

    output_predicate: condition_lo or condition_hi;
    output_value: register_lo;
    output_dst: meta.register_id;
}
action freqIdAction2() {
    freqIdSalu2.execute_stateful_alu_from_hash(hash_heavy_2);
}

table freqIdTable2 {
    actions {
        freqIdAction2;
    }
    default_action: freqIdAction2();
}

table updateTable2 {
    actions {
        updateAction;
    }
    default_action: updateAction();
}

/*---------------------step 3---------------------*/

table dropTable3 {
    actions {
        mydrop;
    }
    default_action: mydrop();
}

blackbox stateful_alu totVotesSalu3 {
    reg: totVotesReg3;
    update_lo_1_value: register_lo + 1;
    output_value: alu_lo;
    output_dst: meta.totVotes;
}
action totVotesAction3() {
    totVotesSalu3.execute_stateful_alu_from_hash(hash_heavy_3);
    modify_field(meta.register_id, 0);
}
table totVotesTable3 {
    actions {
        totVotesAction3;
    }
    default_action: totVotesAction3();
}

table voteDivTable3 {
    actions {
        voteDivAction;
    }
    default_action: voteDivAction();
}

blackbox stateful_alu freqIdSalu3 {
    reg: freqIdReg3;

    condition_hi: meta.totVotes_div >= register_hi;
    condition_lo: myflow.id == register_lo;

    update_lo_1_predicate: condition_lo or condition_hi;
    update_lo_1_value: myflow.id;

    update_hi_1_predicate: condition_lo or condition_hi;
    update_hi_1_value: register_hi + 1;

    output_predicate: condition_lo or condition_hi;
    output_value: register_lo;
    output_dst: meta.register_id;
}
action freqIdAction3() {
    freqIdSalu3.execute_stateful_alu_from_hash(hash_heavy_3);
}

table freqIdTable3 {
    actions {
        freqIdAction3;
    }
    default_action: freqIdAction3();
}

table updateTable3 {
    actions {
        updateAction;
    }
    default_action: updateAction();
}

/*---------------------step 4---------------------*/

table dropTable4 {
    actions {
        mydrop;
    }
    default_action: mydrop();
}

blackbox stateful_alu totVotesSalu4 {
    reg: totVotesReg4;
    update_lo_1_value: register_lo + 1;
    output_value: alu_lo;
    output_dst: meta.totVotes;
}
action totVotesAction4() {
    totVotesSalu4.execute_stateful_alu_from_hash(hash_heavy_4);
    modify_field(meta.register_id, 0);
}
table totVotesTable4 {
    actions {
        totVotesAction4;
    }
    default_action: totVotesAction4();
}

table voteDivTable4 {
    actions {
        voteDivAction;
    }
    default_action: voteDivAction();
}

blackbox stateful_alu freqIdSalu4 {
    reg: freqIdReg4;

    condition_hi: meta.totVotes_div >= register_hi;
    condition_lo: myflow.id == register_lo;

    update_lo_1_predicate: condition_lo or condition_hi;
    update_lo_1_value: myflow.id;

    update_hi_1_predicate: condition_lo or condition_hi;
    update_hi_1_value: register_hi + 1;

    output_predicate: condition_lo or condition_hi;
    output_value: register_lo;
    output_dst: meta.register_id;
}
action freqIdAction4() {
    freqIdSalu4.execute_stateful_alu_from_hash(hash_heavy_4);
}

table freqIdTable4 {
    actions {
        freqIdAction4;
    }
    default_action: freqIdAction4();
}

table updateTable4 {
    actions {
        updateAction;
    }
    default_action: updateAction();
}

/*---------------------step 5---------------------*/

blackbox stateful_alu miceSalu {
     // counter of mice flows
    reg: miceReg;
    update_lo_1_value: register_lo + meta.flow_freq;
}
action miceAction() {
    // action to wrap up miceSalu
    miceSalu.execute_stateful_alu_from_hash(hash_mice);
}
table miceTable {
    // table to wrap up miceAction
    actions {
        miceAction;
    }
    default_action: miceAction();
}

/*--*--* CONTROL BLOCKS *--*--*/

control ingress {
    //step 1
    apply(totVotesTable1);
    apply(voteDivTable1);
    apply(freqIdTable1);
    if(not (meta.register_id == 0)){
        if(meta.register_id == myflow.id) {
            apply(dropTable1);
        }
        apply(updateTable1);
    }

    //step 2
    if(meta.heavy_flag == 1)
    {
        apply(totVotesTable2);
        apply(voteDivTable2);
        apply(freqIdTable2);
        if(not (meta.register_id == 0)){
            if(meta.register_id == myflow.id) {
                apply(dropTable2);
            }
            apply(updateTable2);
        }
    }
    //step 3
    if(meta.heavy_flag == 1)
    {
        apply(totVotesTable3);
        apply(voteDivTable3);
        apply(freqIdTable3);
        if(not (meta.register_id == 0)){
            if(meta.register_id == myflow.id) {
                apply(dropTable3);
            }
            apply(updateTable3);
        }
    }

}

control egress {

    //step 4
    if(meta.heavy_flag == 1)
    {
        apply(totVotesTable4);
        apply(voteDivTable4);
        apply(freqIdTable4);
        if(not (meta.register_id == 0)){
            if(meta.register_id == myflow.id) {
                apply(dropTable4);
            }
            apply(updateTable4);
        }
    }

    //step 5
    if(meta.light_flag == 1)
    {
        apply(miceTable);
    }
}
