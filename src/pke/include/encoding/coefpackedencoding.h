//==================================================================================
// BSD 2-Clause License
//
// Copyright (c) 2014-2022, NJIT, Duality Technologies Inc. and other contributors
//
// All rights reserved.
//
// Author TPOC: contact@openfhe.org
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//==================================================================================

/*
  Represents and defines packing integers of plaintext objects into polynomial coefficients in Palisade
 */

#ifndef SRC_CORE_LIB_ENCODING_COEFPACKEDENCODING_H_
#define SRC_CORE_LIB_ENCODING_COEFPACKEDENCODING_H_

#include <initializer_list>
#include <memory>
#include <vector>

#include "encoding/plaintext.h"

namespace lbcrypto {

class CoefPackedEncoding : public PlaintextImpl {
  std::vector<int64_t> value;

 public:
  // these two constructors are used inside of Decrypt
  CoefPackedEncoding(std::shared_ptr<Poly::Params> vp, EncodingParams ep)
      : PlaintextImpl(vp, ep) {}

  CoefPackedEncoding(std::shared_ptr<NativePoly::Params> vp, EncodingParams ep)
      : PlaintextImpl(vp, ep) {}

  CoefPackedEncoding(std::shared_ptr<DCRTPoly::Params> vp, EncodingParams ep)
      : PlaintextImpl(vp, ep) {}

  CoefPackedEncoding(std::shared_ptr<Poly::Params> vp, EncodingParams ep,
                     std::vector<int64_t> coeffs)
      : PlaintextImpl(vp, ep), value(coeffs) {}

  CoefPackedEncoding(std::shared_ptr<NativePoly::Params> vp, EncodingParams ep,
                     std::vector<int64_t> coeffs)
      : PlaintextImpl(vp, ep), value(coeffs) {}

  CoefPackedEncoding(std::shared_ptr<DCRTPoly::Params> vp, EncodingParams ep,
                     std::vector<int64_t> coeffs)
      : PlaintextImpl(vp, ep), value(coeffs) {}

  virtual ~CoefPackedEncoding() {}

  /**
   * GetCoeffsValue
   * @return the un-encoded scalar
   */
  const std::vector<int64_t>& GetCoefPackedValue() const { return value; }

  /**
   * SetIntVectorValue
   * @param val integer vector to initialize the plaintext
   */
  void SetIntVectorValue(const std::vector<int64_t>& val) { value = val; }

  /**
   * Encode the plaintext into the Poly
   * @return true on success
   */
  bool Encode();

  /**
   * Decode the Poly into the string
   * @return true on success
   */
  bool Decode();

  /**
   * GetEncodingType
   * @return this is a CoefPacked encoding
   */
  PlaintextEncodings GetEncodingType() const { return CoefPacked; }

  /**
   * Get length of the plaintext
   *
   * @return number of elements in this plaintext
   */
  size_t GetLength() const { return value.size(); }

  /**
   * SetLength of the plaintext to the given size
   * @param siz
   */
  void SetLength(size_t siz) { value.resize(siz); }

  /**
   * Method to compare two plaintext to test for equivalence
   * Testing that the plaintexts are of the same type done in operator==
   *
   * @param other - the other plaintext to compare to.
   * @return whether the two plaintext are equivalent.
   */
  bool CompareTo(const PlaintextImpl& other) const {
    const auto& oth = static_cast<const CoefPackedEncoding&>(other);
    return oth.value == this->value;
  }

  /**
   * PrintValue - used by operator<< for this object
   * @param out
   */
  void PrintValue(std::ostream& out) const {
    // for sanity's sake, trailing zeros get elided into "..."
    out << "(";
    size_t i = value.size();
    while (--i > 0)
      if (value[i] != 0) break;

    for (size_t j = 0; j <= i; j++) out << ' ' << value[j];

    out << " ... )";
  }
};

} /* namespace lbcrypto */

#endif /* SRC_CORE_LIB_ENCODING_COEFPACKEDENCODING_H_ */
