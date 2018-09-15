#ifndef STREAMMEASUREMENTSYSTEM_EMFSD_H
#define STREAMMEASUREMENTSYSTEM_EMFSD_H

#include <vector>
#include <cstdint>
#include <cmath>
#include <unordered_map>
#include <iostream>

using std::vector;

class EMFSD
{
    uint32_t w;
    vector<int> counter_dist;
    vector<double> dist_old, dist_new;
public:
    vector<double> ns;
    double n_sum;
    double card_init;
    bool inited = false;
private:
    double n_old, n_new;

    struct BetaGenerator
    {
        int sum;
        int now_flow_num;
        int flow_num_limit;
        vector<int> now_result;

        explicit BetaGenerator(int _sum): sum(_sum)
        {
            now_flow_num = 0;
//            if (sum > 2000) {
//                flow_num_limit = 2;
//                return;
//            }
            if (sum > 600) {
                flow_num_limit = 3;
                return;
            }
            if (sum > 300)
                flow_num_limit = 4;
            else if (sum > 50)
                flow_num_limit = 5;
            else
                flow_num_limit = 7;
        }

        bool get_new_comb()
        {
            for (int j = now_flow_num - 2; j >= 0; --j) {
                int t = ++now_result[j];
                for (int k = j + 1; k < now_flow_num - 1; ++k) {
                    now_result[k] = t;
                }
                int partial_sum = 0;
                for (int k = 0; k < now_flow_num - 1; ++k) {
                    partial_sum += now_result[k];
                }
                int remain = sum - partial_sum;
                if (remain >= now_result[now_flow_num - 2]) {
                    now_result[now_flow_num - 1] = remain;
                    return true;
                }
            }

            return false;
        }

        bool get_next()
        {
            while (now_flow_num < flow_num_limit) {
                switch (now_flow_num) {
                case 0:
                    now_flow_num = 1;
                    now_result.resize(1);
                    now_result[0] = sum;
                    return true;
                case 1:
                    now_flow_num = 2;
                    now_result[0] = 0;
                    // fallthrough
                default:
                    now_result.resize(now_flow_num);
                    if (get_new_comb()) {
//                        for (int t = 0; t < now_flow_num; ++t) {
//                            cout << now_result[t] << " ";
//                        }
//                        cout << endl;
                        return true;
                    } else {
                        now_flow_num++;
                        for (int i = 0; i < now_flow_num - 2; ++i) {
                            now_result[i] = 1;
                        }
                        now_result[now_flow_num - 2] = 0;
                    }
                }
            }

            return false;
        }
    };

    static constexpr int factorial(int n) {
        if (n == 0 || n == 1)
            return 1;
        return factorial(n - 1) * n;
    }

    double get_p_from_beta(BetaGenerator & bt, double lambda, vector<double> & now_dist, double now_n)
    {
        std::unordered_map<uint32_t, uint32_t> mp;
        for (int i = 0; i < bt.now_flow_num; ++i) {
            mp[bt.now_result[i]]++;
        }

        double ret = std::exp(-lambda);
        for (auto & kv: mp) {
            uint32_t fi = kv.second;
            uint32_t si = kv.first;
            double lambda_i = now_n * (now_dist[si]) / w;
            ret *= (std::pow(lambda_i, fi)) / factorial(fi);
        }

        return ret;
    }

    int collect_counters(uint32_t * counters)
    {
        // collect counter information as the dist init
        uint32_t max_counter_val = 0;
        for (uint32_t i = 0; i < w; ++i) {
            max_counter_val = std::max(max_counter_val, counters[i]);
        }
        counter_dist.resize(max_counter_val + 1);
        std::fill(counter_dist.begin(), counter_dist.end(), 0);
        for (uint32_t i = 0; i < w; ++i) {
            counter_dist[counters[i]]++;
        }
/*
        std::cout << w << endl;
        std::cout << counter_dist[0] << endl;
        std::cout << max_counter_val << endl;
*/
        return max_counter_val;
    }

    int collect_counters(uint16_t * counters)
    {
        // collect counter information as the dist init
        uint16_t max_counter_val = 0;
        for (uint32_t i = 0; i < w; ++i) {
            max_counter_val = std::max(max_counter_val, counters[i]);
        }
        counter_dist.resize(max_counter_val + 1);
        std::fill(counter_dist.begin(), counter_dist.end(), 0);
        for (uint32_t i = 0; i < w; ++i) {
            counter_dist[counters[i]]++;
        }
        std::cout << w << endl;
        std::cout << counter_dist[0] << endl;
        std::cout << max_counter_val << endl;

        return max_counter_val;
    }

public:
    EMFSD()
    {
//        BetaGenerator a(15);
//        while (a.get_next()) {
//            for (int j = 0; j < a.now_flow_num; ++j) {
//                cout << a.now_result[j] << " ";
//            }
//            cout << endl;
//        }
    }

    void set_counters(uint32_t _w, uint32_t * counters)
    {
        inited = true;
        w = _w;
        int max_cnt = collect_counters(counters);
        n_new = w - counter_dist[0];
//        std::cout << "w: " << w << std::endl;
//        std::cout << "dist0: " << counter_dist[0] << std::endl;
        dist_new.resize(counter_dist.size());
        ns.resize(counter_dist.size());
        for (uint32_t i = 1; i < counter_dist.size(); ++i) {
            dist_new[i] = counter_dist[i] / double(w - counter_dist[0]);
            ns[i] = counter_dist[i];
        }
    }

    void set_counters(uint32_t _w, uint16_t * counters)
    {
        inited = true;
        w = _w;
        int max_cnt = collect_counters(counters);
        n_new = w - counter_dist[0];
        dist_new.resize(counter_dist.size());
        ns.resize(counter_dist.size());
        for (uint32_t i = 1; i < counter_dist.size(); ++i) {
            dist_new[i] = counter_dist[i] / double(w - counter_dist[0]);
            ns[i] = counter_dist[i];
        }
        card_init = (w * std::log(w / double(counter_dist[0])));
    }

    void next_epoch()
    {
        dist_old = dist_new;
        n_old = n_new;

        double lambda = n_old / double(w);

        std::fill(ns.begin(), ns.end(), 0);

        for (uint32_t i = 1; i < counter_dist.size(); ++i) {
            // enum how to form val:i
            if (counter_dist[i] == 0) {
                continue;
            }
            BetaGenerator bts1(i), bts2(i);
            double sum_p = 0;
            while (bts1.get_next()) {
                double p = get_p_from_beta(bts1, lambda, dist_old, n_old);
                sum_p += p;
            }
            while (bts2.get_next()) {
                double p = get_p_from_beta(bts2, lambda, dist_old, n_old);
                for (int j = 0; j < bts2.now_flow_num; ++j) {
                    ns[bts2.now_result[j]] += counter_dist[i] * p / sum_p;
                }
            }
        }

        n_new = 0;
        for (uint32_t i = 1; i < counter_dist.size(); i++) {
//            ns[i] = int(ns[i]); // TODO
            n_new += ns[i];
        }
        for (uint32_t i = 1; i < counter_dist.size(); i++) {
//            ns[i] = int(ns[i]);
            dist_new[i] = ns[i] / n_new;
        }

        n_sum = n_new;
//        std::cout << ns[1] << std::endl;
    }
};

#endif //STREAMMEASUREMENTSYSTEM_EMFSD_H
