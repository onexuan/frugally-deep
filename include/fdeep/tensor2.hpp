// Copyright 2016, Tobias Hermann.
// https://github.com/Dobiasd/frugally-deep
// Distributed under the MIT License.
// (See accompanying LICENSE file or at
//  https://opensource.org/licenses/MIT)

#pragma once

#include "fdeep/common.hpp"

#include "fdeep/shape2.hpp"
#include "fdeep/tensor2_pos.hpp"

#include <fplus/fplus.hpp>

#include <cassert>
#include <cstddef>
#include <string>
#include <vector>

namespace fdeep { namespace internal
{

class tensor2
{
public:
    tensor2(const shape2& shape, const shared_float_vec& values) :
        shape_(shape),
        values_(values)
    {
        assertion(shape.area() == values->size(), "invalid number of values");
    }
    tensor2(const shape2& shape, float_vec&& values) :
        shape_(shape),
        values_(fplus::make_shared_ref<float_vec>(std::move(values)))
    {
        assertion(shape.area() == values_->size(), "invalid number of values");
    }
    tensor2(const shape2& shape, float_type value) :
        shape_(shape),
        values_(fplus::make_shared_ref<float_vec>(shape.area(), value))
    {
    }

    const float_type& get(const tensor2_pos& pos) const
    {
        return (*values_)[idx(pos)];
    }
    const float_type& get(std::size_t y, std::size_t x) const
    {
        return get(tensor2_pos(y, x));
    }
    float_type& get(const tensor2_pos& pos)
    {
        return (*values_)[idx(pos)];
    }
    float_type& get(std::size_t y, std::size_t x)
    {
        return get(tensor2_pos(y, x));
    }
    void set(const tensor2_pos& pos, float_type value)
    {
        (*values_)[idx(pos)] = value;
    }
    void set(std::size_t y, std::size_t x, float_type value)
    {
        set(tensor2_pos(y, x), value);
    }
    const shape2& shape() const
    {
        return shape_;
    }
    const shared_float_vec& as_vector() const
    {
        return values_;
    }

private:
    std::size_t idx(const tensor2_pos& pos) const
    {
        return
            pos.y_ * shape().width_ +
            pos.x_;
    };
    shape2 shape_;
    shared_float_vec values_;
};

template <typename F>
tensor2 transform_tensor2(F f, const tensor2& m)
{
    return tensor2(m.shape(), fplus::transform(f, m.as_vector()));
}

inline tensor2 multiply(const tensor2& a, const tensor2& b)
{
    assertion(a.shape().width_ == b.shape().height_, "invalid tensor shapes");

    tensor2 m(shape2(a.shape().height_, b.shape().width_),
        static_cast<float_type>(0.0f));

    for (std::size_t i = 0; i < a.shape().height_; ++i)
    {
        for (std::size_t k = 0; k < a.shape().width_; ++k)
        {
            float_type a_i_k = a.get(i, k);
            for (std::size_t j = 0; j < b.shape().width_; ++j)
            {
                m.get(i, j) += a_i_k * b.get(k, j);
            }
        }
    }
    return m;
}

inline tensor2 add_to_tensor2_elems(const tensor2& m, float_type x)
{
    return tensor2(m.shape(), fplus::transform([x](float_type e) -> float_type
    {
        return x + e;
    }, *m.as_vector()));
}

inline tensor2 sub_from_tensor2_elems(const tensor2& m, float_type x)
{
    return tensor2(m.shape(), fplus::transform([x](float_type e) -> float_type
    {
        return e - x;
    }, *m.as_vector()));
}

inline tensor2 multiply_tensor2_elems(const tensor2& m, float_type x)
{
    return tensor2(m.shape(), fplus::transform([x](float_type e) -> float_type
    {
        return x * e;
    }, *m.as_vector()));
}

inline tensor2 divide_tensor2_elems(const tensor2& m, float_type x)
{
    return tensor2(m.shape(), fplus::transform([x](float_type e) -> float_type
    {
        return e / x;
    }, *m.as_vector()));
}

} } // namespace fdeep, namespace internal
