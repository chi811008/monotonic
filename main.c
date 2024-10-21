#include <mach/clock.h>
#include <mach/mach.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

void get_ntp_time() {
    // 使用 ntpdate 獲取 NTP 時間
    FILE *fp = popen("ntpdate -q pool.ntp.org", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error: Failed to run ntpdate\n");
        return;
    }

    // 解析 ntpdate 輸出
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        printf("%s", buffer);
    }

    pclose(fp);

    // 獲取當前時間
    struct timeval tv;
    gettimeofday(&tv, NULL);

    // 打印時間
    printf("NTP Time - Seconds: %ld, Microseconds: %d\n", tv.tv_sec, tv.tv_usec);
}

void get_monotonic_time(mach_timespec_t *mts) {
    clock_serv_t cclock;
    kern_return_t ret;

    // 獲取單調時鐘服務
    ret = host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    if (ret != KERN_SUCCESS) {
        fprintf(stderr, "Error: host_get_clock_service failed\n");
        return;
    }

    // 獲取當前時間
    ret = clock_get_time(cclock, mts);
    if (ret != KERN_SUCCESS) {
        fprintf(stderr, "Error: clock_get_time failed\n");
        return;
    }

    // 釋放時鐘服務
    mach_port_deallocate(mach_task_self(), cclock);
}

int main() {
    mach_timespec_t mts;

    // 獲取並顯示 NTP 時間和 monotonic 時間
    get_ntp_time();
    get_monotonic_time(&mts);
    printf("Monotonic Time - Seconds: %d, Nanoseconds: %d\n", mts.tv_sec, mts.tv_nsec);

    return 0;
}