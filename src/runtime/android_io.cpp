#include "mini_stdint.h"

#define WEAK __attribute__((weak))

extern "C" {

extern void __android_log_vprint(int, const char *, const char *, __builtin_va_list);

extern size_t fwrite(const void *, size_t, size_t, void *);
extern void *fopen(const char *, const char *);
extern int fclose(void *);

WEAK int halide_printf(const char * fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args,fmt);
    __android_log_vprint(7, "halide", fmt, args);
    __builtin_va_end(args);
    // Profiler assumes that return value <=0 is failure,
    // so return something else here.
    return 1;
}

static bool write_stub(const void *bytes, size_t size, void *f) {
    return fwrite(const_cast<void *>(bytes), size, 1, f) == 1;
}

WEAK int32_t halide_debug_to_file(const char *filename, uint8_t *data,
                                  int32_t s0, int32_t s1, int32_t s2, int32_t s3,
                                  int32_t type_code, int32_t bytes_per_element) {
    void *f = fopen(filename, "wb");
    if (!f) return -1;

    int result = halide_write_debug_image(filename, data, s0, s1, s2, s3,
					  type_code, bytes_per_element, write_stub, (void *)f);

    fclose(f);
    return result;
}

}
