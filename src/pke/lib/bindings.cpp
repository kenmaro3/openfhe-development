#include "openfhe.h"
#include "stdio.h"
#include <emscripten/bind.h>
#include <vector>

using namespace lbcrypto;
using namespace std;
using namespace emscripten;

int main() {
    printf("yoyoyo, hello dude\n");
    return 0;
}

// /*
//   Transform a JS TypedArray into a Vector of the appropriate type (zero copy)
// */
template <typename T>
std::vector<T> vecFromJSArrayFast(const val& v) {
    std::vector<T> rv;
    const size_t l = v["length"].as<std::size_t>();
    rv.resize(l);
    val memoryView{typed_memory_view(l, rv.data())};
    memoryView.call<void>("set", v);
    return rv;
};

// std::vector<double> vecFromJSArrayFastDouble(const val& v) {
//     std::vector<double> rv;
//     const size_t l = v["length"].as<std::size_t>();
//     rv.resize(l);
//     val memoryView{typed_memory_view(l, rv.data())};
//     memoryView.call<void>("set", v);
//     return rv;
// };
// template<typename T>
// vector<T> copyToVector(const val &typedArray)
// {
//   std::vector<T> vec;
//   unsigned int length = typedArray["length"].as<unsigned int>();
//   val memory = val::module_property("buffer");

//   vec.reserve(length);

//   val memoryView = typedArray["constructor"].new_(memory, reinterpret_cast<uintptr_t>(vec.data()), length);

//   memoryView.call<void>("set", typedArray);

//   return vec;
// }

/*
  Converts a Vector to a JS TypedArray (zero copy)
*/
template <typename T>
emscripten::val jsArrayFromVec(const std::vector<T>& vec) {
    const size_t l = vec.size();
    return val(typed_memory_view(l, vec.data()));
};

float lerp(float a, float b, float t) {
    return (1 - t) * a + t * b;
}

CryptoContext<DCRTPoly> generate_context_from_parameters(const CCParams<CryptoContextCKKSRNS>& params) {
    CryptoContext<DCRTPoly> cc = CryptoContextCKKSRNS::genCryptoContext(params);
    //return *cc.get();
    return cc;
}

// CryptoContextImpl<DCRTPoly> test_generate(){
//     CCParams<CryptoContextCKKSRNS> parameters;
//     SecretKeyDist secretKeyDist = UNIFORM_TERNARY;
//     parameters.SetSecretKeyDist(secretKeyDist);
//     parameters.SetSecurityLevel(HEStd_NotSet);
//     parameters.SetRingDim(1 << 12);

//     auto cc = CryptoContextCKKSRNS::genCryptoContext(parameters);
//     return *cc.get();
// }

EMSCRIPTEN_BINDINGS(OPENFHE) {
    emscripten::function("jsArrayUint8FromVec", select_overload<val(const std::vector<uint8_t>&)>(&jsArrayFromVec));
    emscripten::function("lerp", &lerp);
    emscripten::function("generate_context_from_parameters", generate_context_from_parameters);
    // emscripten::function("vecFromArrayFloat64", select_overload<std::vector<double>(const val&)>(&vecFromJSArrayFast));
    //emscripten::function("vecFromArrayFloat64", &vecFromJSArrayFastDouble);
    // emscripten::function("test_generate", test_generate);

    enum_<PKESchemeFeature>("PKESchemeFeature")
        .value("PKE", PKESchemeFeature::PKE)
        .value("KEYSWITCH", PKESchemeFeature::KEYSWITCH)
        .value("PRE", PKESchemeFeature::PRE)
        .value("LEVELEDSHE", PKESchemeFeature::LEVELEDSHE)
        .value("ADVANCEDSHE", PKESchemeFeature::ADVANCEDSHE)
        .value("MULTIPARTY", PKESchemeFeature::MULTIPARTY)
        .value("FHE", PKESchemeFeature::FHE);

    enum_<ScalingTechnique>("ScalingTechnique")
        .value("FIXEDMANUAL", ScalingTechnique::FIXEDMANUAL)
        .value("FIXEDAUTO", ScalingTechnique::FIXEDAUTO)
        .value("FLEXIBLEAUTO", ScalingTechnique::FLEXIBLEAUTO)
        .value("NORESCALE", ScalingTechnique::NORESCALE);

    enum_<SecurityLevel>("SecurityLevel")
        .value("HEStd_128_classic", SecurityLevel::HEStd_128_classic)
        .value("HEStd_192_classic", SecurityLevel::HEStd_192_classic)
        .value("HEStd_256_classic", SecurityLevel::HEStd_256_classic)
        .value("HEStd_NotSet", SecurityLevel::HEStd_NotSet);

    enum_<SecretKeyDist>("SecretKeyDist")
        .value("GAUSSIAN", SecretKeyDist::GAUSSIAN)
        .value("UNIFORM_TERNARY", SecretKeyDist::UNIFORM_TERNARY)
        .value("SPARSE_TERNARY", SecretKeyDist::SPARSE_TERNARY);

    class_<Params>("Params")
        .constructor<>()
        .function("SetRingDim", &Params::SetRingDim)
        .function("SetSecurityLevel", &Params::SetSecurityLevel)
        .function("SetSecretKeyDist", &Params::SetSecretKeyDist)
        .function("SetScalingModSize", &Params::SetScalingModSize)
        .function("SetFirstModSize", &Params::SetFirstModSize)
        .function("SetScalingTechnique", &Params::SetScalingTechnique)
        .function("SetMultiplicativeDepth", &Params::SetMultiplicativeDepth);

    class_<CCParams<CryptoContextCKKSRNS>, base<Params>>("CryptoParameters")
        .constructor<>()
        .function("SetRingDim", &CCParams<CryptoContextCKKSRNS>::SetRingDim)
        .function("SetSecurityLevel", &CCParams<CryptoContextCKKSRNS>::SetSecurityLevel)
        .function("SetSecretKeyDist", &CCParams<CryptoContextCKKSRNS>::SetSecretKeyDist)
        .function("SetScalingModSize", &CCParams<CryptoContextCKKSRNS>::SetScalingModSize)
        .function("SetFirstModSize", &CCParams<CryptoContextCKKSRNS>::SetFirstModSize)
        .function("SetMultiplicativeDepth", &CCParams<CryptoContextCKKSRNS>::SetMultiplicativeDepth)
        .function("GetBootstrapDepth",
                  optional_override([](CCParams<CryptoContextCKKSRNS>& self, int levelsUsedBeforeBootstrap,
                                       SecretKeyDist secretKeyDist) {
                      uint32_t approxBootstrapDepth     = 8;
                      std::vector<uint32_t> levelBudget = {4, 4};
                      usint depth                       = levelsUsedBeforeBootstrap +
                                    FHECKKSRNS::GetBootstrapDepth(approxBootstrapDepth, levelBudget, secretKeyDist);
                      return depth;
                  }));

    // class_<std::shared_ptr<CryptoContextImpl<DCRTPoly>>>("CryptoContext")
    //     .function("Enable",
    //               (std::shared_ptr<CryptoContextImpl<DCRTPoly>> & self){return self->Enable(PKESchemeFeature::PKE)});

    class_<CryptoContextImpl<DCRTPoly>>("CryptoContextImpl")
        //.function("Enable", select_overload<void(PKESchemeFeature)>(&CryptoContextImpl<DCRTPoly>::Enable))
        .smart_ptr_constructor("CryptoContextImpl", &std::make_shared<CryptoContextImpl<DCRTPoly>>)
        .function("Enable", optional_override([](CryptoContextImpl<DCRTPoly>& self, PKESchemeFeature feature) {
                      self.Enable(feature);
                  }))
        .function("GetRingDimension", (&CryptoContextImpl<DCRTPoly>::GetRingDimension))
        .function("EvalBootstrapSetup", optional_override([](CryptoContextImpl<DCRTPoly>& self) {
                      std::vector<uint32_t> levelBudget = {4, 4};
                      self.EvalBootstrapSetup(levelBudget);
                  }))
        .function("KeyGen", &CryptoContextImpl<DCRTPoly>::KeyGen)
        .function("EvalMultKeyGen", &CryptoContextImpl<DCRTPoly>::EvalMultKeyGen)
        .function("EvalBootstrapKeyGen", &CryptoContextImpl<DCRTPoly>::EvalBootstrapKeyGen)
        .function("ReEncrypt", &CryptoContextImpl<DCRTPoly>::ReEncrypt)

        .function("ReKeyGen",
                  optional_override(
                      [](CryptoContextImpl<DCRTPoly>& self, PrivateKey<DCRTPoly> privateKeyFrom,
                         PublicKey<DCRTPoly> publicKeyTo) { return self.ReKeyGen(privateKeyFrom, publicKeyTo); }))

        // .function("MakeCKKSPackedPlaintext", select_overload<Plaintext(const std::vector<double> &, size_t, uint32_t, std::shared_ptr<ParmType>, usint) const>(
        //                                          &CryptoContextImpl<DCRTPoly>::MakeCKKSPackedPlaintext));
        //.function("MakeCKKSPackedPlaintext", &CryptoContextImpl<DCRTPoly>::MakeCKKSPackedPlaintextTmp)

        .function("EvalRotateKeyGen", optional_override([](CryptoContextImpl<DCRTPoly>& self,
                                                           PrivateKey<DCRTPoly> secretKey, const val& v) {
                      std::vector<int> test = vecFromJSArrayFast<int>(v);
                      self.EvalRotateKeyGen(secretKey, test);
                  }))

        .function("MakeCKKSPackedPlaintext", optional_override([](CryptoContextImpl<DCRTPoly>& self, const val& v) {
                      std::vector<double> test = vecFromJSArrayFast<double>(v);
                      return self.MakeCKKSPackedPlaintextTmp(test);
                  }))

        .function("Encrypt", select_overload<Ciphertext<DCRTPoly>(const PublicKey<DCRTPoly>, Plaintext) const>(
                                 &CryptoContextImpl<DCRTPoly>::Encrypt))

        .function(
            "ReEncrypt",
            optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c,
                                 shared_ptr<EvalKeyImpl<DCRTPoly>> evalKey) { return self.ReEncrypt(c, evalKey); }))

        // .function("Decrypt", &CryptoContextImpl<DCRTPoly>::Decrypt)
        // .function("Decrypt",
        //           select_overload<DecryptResult(ConstCiphertext<DCRTPoly>, const PrivateKey<DCRTPoly>, Plaintext*)>(
        //               &CryptoContextImpl<DCRTPoly>::Decrypt),
        //           allow_raw_pointers())

        .function("Decrypt",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c,
                                       PrivateKey<DCRTPoly> privateKey) {
                      Plaintext res_c;
                      self.Decrypt(privateKey, c, &res_c);
                      return res_c;
                  }))

        .function("EvalAdd",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c1,
                                       shared_ptr<CiphertextImpl<DCRTPoly>> c2) { return self.EvalAdd(c1, c2); }))

        .function("EvalSub",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c1,
                                       shared_ptr<CiphertextImpl<DCRTPoly>> c2) { return self.EvalSub(c1, c2); }))

        .function("EvalMult",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c,
                                       double s) { return self.EvalMult(c, s); }))

        .function("EvalMultAndRelinearize",
                  optional_override(
                      [](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c1,
                         shared_ptr<CiphertextImpl<DCRTPoly>> c2) { return self.EvalMultAndRelinearize(c1, c2); }))

        .function("EvalRotate",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c,
                                       int num) { return self.EvalRotate(c, num); }))

        .function("EvalBootstrap",
                  optional_override([](CryptoContextImpl<DCRTPoly>& self, shared_ptr<CiphertextImpl<DCRTPoly>> c) {
                      return self.EvalBootstrap(c);
                  }));

    //.function("Enable", select_overload<void(PKESchemeFeature)>(&CryptoContextImpl<DCRTPoly>::Enable))
    class_<Plaintext>("Plaintext")
        .constructor<>()
        // .smart_ptr_constructor("PlaintextImpl", &std::make_shared<PlaintextImpl>)
        .function("SetLength", optional_override([](Plaintext& self, int length) { self->SetLength(length); }))
        .function("Print", optional_override([](Plaintext& self) { std::cout << self << std::endl; }));

    // class_<PlaintextImpl>("PlaintextImpl")
    //     // .smart_ptr_constructor("PlaintextImpl", &std::make_shared<PlaintextImpl>)
    //     .function("SetLength", &PlaintextImpl::SetLength)
    //     .function("Print", optional_override([](PlaintextImpl& self) { std::cout << self << std::endl; }));

    class_<CiphertextImpl<DCRTPoly>>("CiphertextImpl")
        .smart_ptr_constructor("CiphertextImpl", &std::make_shared<CiphertextImpl<DCRTPoly>>);

    class_<KeyPair<DCRTPoly>>("KeyPair")
        .constructor<>()
        // .constructor<PublicKey<DCRTPoly>, PrivateKey<DCRTPoly>>()
        .property("publicKey", &KeyPair<DCRTPoly>::publicKey)
        .property("secretKey", &KeyPair<DCRTPoly>::secretKey);

    class_<PublicKeyImpl<DCRTPoly>>("PublicKey")
        .smart_ptr_constructor("PublicKeyImpl", &std::make_shared<PublicKeyImpl<DCRTPoly>>);

    class_<PrivateKeyImpl<DCRTPoly>>("PrivateKey")
        .smart_ptr_constructor("PrivateKeyImpl", &std::make_shared<PrivateKeyImpl<DCRTPoly>>);

    class_<EvalKeyImpl<DCRTPoly>>("EvalKey").smart_ptr_constructor("EvalKeyImpl",
                                                                   &std::make_shared<EvalKeyImpl<DCRTPoly>>);

    // class_<DecryptResult>("DecryptResult");
    // .constructor<>();
    //.function("Enable", select_overload<void(PKESchemeFeature)>(&CryptoContextImpl<DCRTPoly>::Enable));
}