/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef COUNTERS_COUNTERRECORD_HH_2338794186__H_
#define COUNTERS_COUNTERRECORD_HH_2338794186__H_


#include <sstream>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#include "boost/any.hpp"
#include "avro/Specific.hh"
#include "avro/Encoder.hh"
#include "avro/Decoder.hh"
#pragma GCC diagnostic pop

namespace counters {
struct Counter {
    boost::array<uint8_t, 40> key;
    int64_t by;
    Counter() :
        key(boost::array<uint8_t, 40>()),
        by(int64_t())
        { }
};

}
namespace avro {
template<> struct codec_traits<counters::Counter> {
    static void encode(Encoder& e, const counters::Counter& v) {
        avro::encode(e, v.key);
        avro::encode(e, v.by);
    }
    static void decode(Decoder& d, counters::Counter& v) {
        if (avro::ResolvingDecoder *rd =
            dynamic_cast<avro::ResolvingDecoder *>(&d)) {
            const std::vector<size_t> fo = rd->fieldOrder();
            for (std::vector<size_t>::const_iterator it = fo.begin();
                it != fo.end(); ++it) {
                switch (*it) {
                case 0:
                    avro::decode(d, v.key);
                    break;
                case 1:
                    avro::decode(d, v.by);
                    break;
                default:
                    break;
                }
            }
        } else {
            avro::decode(d, v.key);
            avro::decode(d, v.by);
        }
    }
};

}
#endif
