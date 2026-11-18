/* SPDX-License-Identifier: BSD-2-Clause */
#pragma once

#include <QImage>
#include <QtClassHelperMacros>
#include <functional>
#include <utility>

namespace KImageFormats
{
struct DecodeFeedback {
    std::function<bool()> cancelled;
    std::function<void(const QImage &)> preview;
    bool delivered = false;

    bool stopped() const { return cancelled && cancelled(); }

    void offer(const QImage &image)
    {
        if (!delivered && preview && !stopped()) {
            delivered = true;
            preview(image);
        }
    }
};

struct DecodeContext {
    DecodeFeedback *feedback = nullptr;
    int threads = 0;
};

inline thread_local DecodeContext decodeContext;

class DecodeFeedbackScope
{
public:
    DecodeFeedbackScope(DecodeFeedback &feedback, int threads)
        : previous(std::exchange(decodeContext, {&feedback, threads}))
    {
    }

    ~DecodeFeedbackScope() { decodeContext = previous; }
    Q_DISABLE_COPY_MOVE(DecodeFeedbackScope)

private:
    const DecodeContext previous;
};
}
