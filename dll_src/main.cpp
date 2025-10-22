#include <algorithm>
#include <execution>
#include <ranges>
#include <vector>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <filter2.h>
#include <module2.h>

#ifndef VERSION
#define VERSION L"0.1.0"
#endif

void
sort(SCRIPT_MODULE_PARAM *p) {
    int n = p->get_param_num();
    if (n != 5) {
        p->set_error("Incorrect number of arguments");
        return;
    }

    auto px = reinterpret_cast<PIXEL_RGBA *>(p->get_param_data(0));
    const auto meta = reinterpret_cast<PIXEL_RGBA *>(p->get_param_data(1));
    const int w = p->get_param_int(2);
    const int h = p->get_param_int(3);
    const bool mask = p->get_param_int(4);

    const auto rows = std::views::iota(0, h);
    std::for_each(std::execution::par, rows.begin(), rows.end(), [&](int y) {
        const int ofs = y * w;
        const PIXEL_RGBA *data = meta + ofs;
        PIXEL_RGBA *dst = px + ofs;

        std::vector<PIXEL_RGBA> buf(dst, dst + w);
        std::vector<int> idx(std::from_range, std::views::iota(0, w));

        int st = 0;
        for (int x = 0; x < w; ++x) {
            if (data[x].a)
                continue;

            if (st < x)
                std::sort(std::execution::unseq, idx.begin() + st, idx.begin() + x,
                          [&](int a, int b) { return data[a].r < data[b].r; });

            st = x + 1;
        }

        if (st < w)
            std::sort(std::execution::unseq, idx.begin() + st, idx.end(),
                      [&](int a, int b) { return data[a].r < data[b].r; });

        if (mask)
            for (int x = 0; x < w; ++x) dst[x] = data[x].a ? buf[idx[x]] : PIXEL_RGBA{};
        else
            for (int x = 0; x < w; ++x) dst[x] = buf[idx[x]];
    });
}

static SCRIPT_MODULE_FUNCTION functions[] = {{L"sort", sort}, {nullptr}};

static SCRIPT_MODULE_TABLE script_module_table = {L"PixelSorter_K v" VERSION L" by Korarei", functions};

extern "C" SCRIPT_MODULE_TABLE *
GetScriptModuleTable(void) {
    return &script_module_table;
}
