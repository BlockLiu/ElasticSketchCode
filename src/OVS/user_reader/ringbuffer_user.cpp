#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h> /* For mode constants */
#include <fcntl.h> /* For O_* constants */ 
#include <errno.h>
#include <semaphore.h>
#include <sys/ioctl.h>  /* ioctl */

#include "util.h"

#include "ringbuffer.h"

ringbuffer_t* create_ringbuffer_shm(const char* name, uint64_t tuple_size) {

    LOG_MSG("This is a normal ringbuffer\n");

    // allocate ringbuffer_t*
    ringbuffer_t* ret = (ringbuffer_t*)malloc(sizeof(struct RingBuffer));
    if (ret == NULL) {
        LOG_ERR("ringbuffer_t* malloc() failed: %s\n", strerror(errno));
    }

    // setup name, fd, tuple size
    ret->name = (char*)malloc(strlen(name) + 1);
    if (ret == NULL) {
        LOG_ERR("ringbuffer_t* name malloc() failed: %s\n", strerror(errno));
    }
    strcpy(ret->name, name);
    ret->name[strlen(name)] = 0;
    ret->fd = -1;
    ret->tuple_size = tuple_size;
    
    // create shared memory device
    int shm_fd = -1;
    shm_unlink(name);
    shm_fd = shm_open(name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd == -1) {
        LOG_ERR("shm_open() failed: %s\n", strerror(errno));
    }
    
    // set shared memory size
    uint64_t shared_size = sizeof(sem_t) + sizeof(struct RBMeta) + sizeof(struct RBAppro) + RB_SIZE * tuple_size;
    if (ftruncate(shm_fd, shared_size) == -1) {
        shm_unlink(name);
        LOG_ERR("ftruncate() failed: %s\n", strerror(errno));
    }
    
    // map into our space
    void* ptr = mmap(0, shared_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        shm_unlink(name);
        LOG_ERR("mmap() failed: %s\n", strerror(errno));
    }

    // set up pointers to the shared memory region
    ret->meta = (struct RBMeta*)((unsigned char*)ptr + sizeof(sem_t));
    ret->appro = (struct RBAppro*)((unsigned char*)ptr + sizeof(sem_t) + sizeof(struct RBMeta));
    ret->data = (unsigned char*)ptr + sizeof(sem_t) + sizeof(struct RBMeta) + sizeof(struct RBAppro);
    
    // init ring buffer, require lock the resouces
    sem_t* sem = (sem_t*) ptr;
    if (sem_init(sem, 1, 0) == -1) {
        shm_unlink(name);
        LOG_ERR("sem_init() failed: %s\n", strerror(errno));
    }
    
    memset(ret->appro, 0, sizeof(struct RBAppro));
    ret->meta->readIndex = 0; 
    ret->meta->writeIndex = 0;
    ret->meta->localWriteIndex = 0;
    ret->meta->nextReadIndex = 0;
    ret->meta->rBatchCount = 0;
    ret->meta->localReadIndex = 0;
    ret->meta->nextWriteIndex = 0;
    ret->meta->wBatchCount = 0;
    memset(ret->data, 0, RB_SIZE * tuple_size);
    
    // compelte init
    sem_post(sem);
    
    return ret;
}

ringbuffer_t* connect_ringbuffer_shm(const char* name, uint64_t tuple_size) {

    // allocate ringbuffer_t*
    ringbuffer_t* ret = (ringbuffer_t*)malloc(sizeof(struct RingBuffer));
    if (ret == NULL) {
        LOG_ERR("ringbuffer_t* malloc() failed: %s\n", strerror(errno));
    }

    // setup name, fd, tuple size
    ret->name = (char*)malloc(strlen(name) + 1);
    if (ret == NULL) {
        LOG_ERR("ringbuffer_t* name malloc() failed: %s\n", strerror(errno));
    }
    strcpy(ret->name, name);
    ret->name[strlen(name)] = 0;
    ret->fd = -1;
    ret->tuple_size = tuple_size;
    
    int shm_fd = -1;
    while ((shm_fd = shm_open(name, O_RDWR, S_IRUSR | S_IWUSR)) == -1);
 	if (shm_fd == -1)
        LOG_ERR("shm_open() failed: %s\n", strerror(errno));

    // set shared memory size
    uint64_t shared_size = sizeof(sem_t) + sizeof(struct RBMeta) + sizeof(struct RBAppro) + RB_SIZE * tuple_size;
    if (ftruncate(shm_fd, shared_size) == -1) {
        shm_unlink(name);
        LOG_ERR("ftruncate() failed: %s\n", strerror(errno));
    }

    // map into our space
    void* ptr = mmap(0, shared_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (ptr == MAP_FAILED) {
        shm_unlink(name);
        LOG_ERR("mmap() failed: %s\n", strerror(errno));
    }

    // set up pointers to the shared memory region
    ret->meta = (struct RBMeta*)((unsigned char*)ptr+sizeof(sem_t));
    ret->appro = (struct RBAppro*)((unsigned char*)ptr + sizeof(sem_t) + sizeof(struct RBMeta));
    ret->data = (unsigned char*)ptr + sizeof(sem_t) + sizeof(struct RBMeta) + sizeof(struct RBAppro);

	return ret;

    // wait for signal to init the ring buffer
    sem_t* sem = (sem_t*) ptr;
    if (sem_wait(sem) == -1) {
        shm_unlink(name);
        LOG_ERR("sem_wait() failed: %s\n", strerror(errno));
    }
    if (shm_unlink(name) == -1)
        LOG_ERR("shm_unlink() failed: %s\n", strerror(errno));

    return ret;
}

ringbuffer_t* connect_ringbuffer_kshm(const char* name, uint64_t tuple_size) {

	LOG_MSG("Connect ringbuffer in kernel mode.\n");

    // open device
  	int dev_fd = open(name, O_RDWR);
	if (dev_fd < 0) {
        LOG_ERR("Open device file failed: %s, %s\n", name, strerror(errno));
	}
    
    // map into our space
    unsigned long mmap_size = sizeof(struct RBMeta)+sizeof(struct RBAppro)+tuple_size*RB_SIZE;
  	void* ptr = mmap(NULL, mmap_size, PROT_READ|PROT_WRITE, MAP_SHARED, dev_fd, 0);
    if (ptr == MAP_FAILED) {
        close(dev_fd);
        LOG_ERR("mmap() failed: %s, %s\n", name, strerror(errno));
    }

    // allocate ringbuffer_t*
    ringbuffer_t* ret = (ringbuffer_t*)malloc(sizeof(struct RingBuffer));
    if (ret == NULL) {
        close(dev_fd);
        LOG_ERR("ringbuffer_t* malloc() failed: %s\n", strerror(errno));
    }

    // setup name, fd, tuple size
    ret->name = NULL;
    ret->fd = dev_fd;
    ret->tuple_size = tuple_size;

    // set up pointers to the shared memory region
    ret->meta = (struct RBMeta*)ptr;
    ret->appro = (struct RBAppro*)((unsigned char*)ptr + sizeof(struct RBMeta));
    ret->data = (unsigned char*)ptr +
        sizeof(struct RBMeta) + sizeof(struct RBAppro);
    return ret;
}

void start_ringbuffer_kshm(ringbuffer_t* rb) {
	int r = ioctl(rb->fd, 1, START);
    if (r == -1) {
	    close(rb->fd);
		LOG_ERR("ioctl error: %s\n", strerror(errno));
    }
}

void stop_ringbuffer_kshm(ringbuffer_t* rb) {
	int r = ioctl(rb->fd, 1, STOP);
    if (r != 0) {
	    close(rb->fd);
		LOG_ERR("ioctl error: %s\n", strerror(errno));
    }
}

void close_ringbuffer_shm(ringbuffer_t* rb) {
    shm_unlink(rb->name);
    char* ptr = (char*)rb->meta-sizeof(sem_t);
    uint64_t shared_size = sizeof(sem_t) + sizeof(struct RBMeta) + sizeof(struct RBAppro) + RB_SIZE * rb->tuple_size;
    munmap(ptr, shared_size);
    free(rb->name);
    free(rb);
}

void close_ringbuffer_kshm(ringbuffer_t* rb) {
    //char* ptr = (char*)rb->meta-sizeof(sem_t);
    //uint64_t shared_size = sizeof(sem_t) + sizeof(struct RBMeta) + RB_SIZE * rb->tuple_size;
    //munmap(ptr, shared_size);
	close(rb->fd);
    free(rb);
}

void flush_ringbuffer(ringbuffer_t* rb) {
    rb->meta->writeIndex = rb->meta->nextWriteIndex;
	rb->meta->readIndex = rb->meta->nextReadIndex;
}

void read_complete_ringbuffer(ringbuffer_t* rb) {
	rb->meta->readIndex = rb->meta->nextReadIndex;
	rb->meta->localReadIndex = rb->meta->nextReadIndex;
}

int  ringbuffer_size(ringbuffer_t* rb) {
    if (rb->meta->nextWriteIndex < rb->meta->localReadIndex) {
        return rb->meta->nextWriteIndex-rb->meta->localReadIndex+RB_SIZE;
    }
    else {
        return rb->meta->nextWriteIndex-rb->meta->localReadIndex;
    }
}

// 0: success; otherwise: fail
int write_ringbuffer(ringbuffer_t* rb, void* data, unsigned long size) {
	int afterNextWriteIndex = nextVal(rb->meta->nextWriteIndex);
	if (afterNextWriteIndex == rb->meta->localReadIndex) {
		if (afterNextWriteIndex == rb->meta->readIndex) {
            return -1;
		}
		rb->meta->localReadIndex = rb->meta->readIndex;
	}
    memcpy(rb->data+rb->tuple_size*rb->meta->nextWriteIndex, data, size);
	rb->meta->nextWriteIndex = afterNextWriteIndex;
	rb->meta->wBatchCount++;
	if (rb->meta->wBatchCount >= RB_BATCH) {
		rb->meta->writeIndex = rb->meta->nextWriteIndex;
		rb->meta->wBatchCount = 0;
	}
    return 0;
}

// 0: success; otherwise: fail
int write_ringbuffer_block(ringbuffer_t* rb, void* data, unsigned long size) {
	int afterNextWriteIndex = nextVal(rb->meta->nextWriteIndex);
	if (afterNextWriteIndex == rb->meta->localReadIndex) {
		if (afterNextWriteIndex == rb->meta->readIndex) {
            return -1;
		}
		rb->meta->localReadIndex = rb->meta->readIndex;
	}
    memcpy(rb->data+rb->tuple_size*rb->meta->nextWriteIndex, data, size);
	rb->meta->nextWriteIndex = afterNextWriteIndex;
	rb->meta->wBatchCount++;
	if (rb->meta->wBatchCount >= RB_BATCH) {
		rb->meta->writeIndex = rb->meta->nextWriteIndex;
		rb->meta->wBatchCount = 0;
	}
    return 0;
}

// 0: success; otherwise: fail
int read_ringbuffer(ringbuffer_t* rb, void* data) {
	if (rb->meta->nextReadIndex == rb->meta->localWriteIndex) {
		if (rb->meta->nextReadIndex == rb->meta->writeIndex) {
                return -1;
		}
		rb->meta->localWriteIndex = rb->meta->writeIndex;
	}

    memcpy(data, rb->data+rb->meta->nextReadIndex*rb->tuple_size, rb->tuple_size);
	rb->meta->nextReadIndex = nextVal(rb->meta->nextReadIndex);
	rb->meta->rBatchCount++;
	if (rb->meta->rBatchCount >= RB_BATCH) {
		rb->meta->readIndex = rb->meta->nextReadIndex;
		rb->meta->rBatchCount = 0;
	}
    return 0;
}

void ringbuffer_stat_appro(ringbuffer_t* rb, const char* filename) {}
void ringbuffer_export_appro(ringbuffer_t* rb, const char* filename) {}
void ringbuffer_reset_appro(ringbuffer_t* rb) {}
