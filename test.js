
console.log("first called");

const test_basic_operation = () => {
  const parameters = new Module.CryptoParameters();
  parameters.SetRingDim(1 << 12);
  parameters.SetSecurityLevel(Module.SecurityLevel.HEStd_NotSet)
  parameters.SetScalingModSize(59)
  parameters.SetScalingTechnique(Module.ScalingTechnique.FLEXIBLEAUTO)
  parameters.SetFirstModSize(60)
  const depth = parameters.GetBootstrapDepth(1, Module.SecretKeyDist.UNIFORM_TERNARY)
  parameters.SetMultiplicativeDepth(depth)
  console.log("depth", depth)
  console.log("parameters okay")
  console.log(parameters)

  const cc = Module.generate_context_from_parameters(parameters)
  cc.Enable(Module.PKESchemeFeature.PKE)
  cc.Enable(Module.PKESchemeFeature.PRE)
  cc.Enable(Module.PKESchemeFeature.KEYSWITCH)
  cc.Enable(Module.PKESchemeFeature.LEVELEDSHE)
  cc.Enable(Module.PKESchemeFeature.ADVANCEDSHE)
  cc.Enable(Module.PKESchemeFeature.FHE)

  const ringDim = cc.GetRingDimension()
  const numSlots = ringDim / 2
  console.log("ringDim", ringDim)
  console.log("numSlots", numSlots)
  cc.EvalBootstrapSetup()


  const keyPair = cc.KeyGen()
  // const keyPair = new Module.KeyPair()
  cc.EvalMultKeyGen(keyPair.secretKey)
  cc.EvalBootstrapKeyGen(keyPair.secretKey, numSlots)

  const rotateIndex = new Int32Array([1, 2, 3])
  cc.EvalRotateKeyGen(keyPair.secretKey, rotateIndex)

  const p1 = new Float64Array([1, 2, 3]);
  const p2 = new Float64Array([1, 0, 0]);
  const ptxt1 = cc.MakeCKKSPackedPlaintext(p1)
  const ptxt2 = cc.MakeCKKSPackedPlaintext(p2)
  ptxt1.SetLength(3)
  // console.log("print")
  ptxt1.Print()

  const c1 = cc.Encrypt(keyPair.publicKey, ptxt1)
  const c2 = cc.Encrypt(keyPair.publicKey, ptxt2)

  const ctxt_bs = cc.EvalBootstrap(c1)

  const ctxt_add = cc.EvalAdd(c1, c2)
  const ctxt_sub = cc.EvalSub(c1, c2)
  const ctxt_mult = cc.EvalMultAndRelinearize(c1, c2)
  const ctxt_mult_s = cc.EvalMult(c1, 2.0)
  const ctxt_rotate = cc.EvalRotate(c1, 1)

  const dec = cc.Decrypt(ctxt_bs, keyPair.secretKey)
  const d_add = cc.Decrypt(ctxt_add, keyPair.secretKey)
  const d_sub = cc.Decrypt(ctxt_sub, keyPair.secretKey)
  const d_mult = cc.Decrypt(ctxt_mult, keyPair.secretKey)
  const d_mult_s = cc.Decrypt(ctxt_mult_s, keyPair.secretKey)
  const d_rotate = cc.Decrypt(ctxt_rotate, keyPair.secretKey)
  dec.SetLength(3)
  dec.Print()
  d_add.SetLength(3)
  d_add.Print()
  d_sub.SetLength(3)
  d_sub.Print()
  d_mult.SetLength(3)
  d_mult.Print()
  d_mult_s.SetLength(3)
  d_mult_s.Print()
  d_rotate.SetLength(3)
  d_rotate.Print()
}

const test_pre = () => {
  const parameters = new Module.CryptoParameters();
  parameters.SetRingDim(1 << 12);
  parameters.SetSecurityLevel(Module.SecurityLevel.HEStd_NotSet)
  parameters.SetScalingModSize(59)
  parameters.SetScalingTechnique(Module.ScalingTechnique.FLEXIBLEAUTO)
  parameters.SetFirstModSize(60)
  const depth = parameters.GetBootstrapDepth(1, Module.SecretKeyDist.UNIFORM_TERNARY)
  parameters.SetMultiplicativeDepth(depth)
  console.log("depth", depth)
  console.log("parameters okay")
  console.log(parameters)

  const cc = Module.generate_context_from_parameters(parameters)
  cc.Enable(Module.PKESchemeFeature.PKE)
  cc.Enable(Module.PKESchemeFeature.PRE)


  const kp1 = cc.KeyGen()
  const kp2 = cc.KeyGen()
  const reKey12 = cc.ReKeyGen(kp1.secretKey, kp2.publicKey)

  const p1 = new Float64Array([1, 2, 3]);
  const p2 = new Float64Array([1, 0, 0]);
  const ptxt1 = cc.MakeCKKSPackedPlaintext(p1)
  ptxt1.SetLength(3)
  ptxt1.Print()

  const c1 = cc.Encrypt(kp1.publicKey, ptxt1)

  const c2 = cc.ReEncrypt(c1, reKey12)

  const dec1 = cc.Decrypt(c1, kp1.secretKey)
  const dec2 = cc.Decrypt(c2, kp2.secretKey)

  dec1.SetLength(3)
  dec1.Print()
  dec2.SetLength(3)
  dec2.Print()
}

var Module = {
  onRuntimeInitialized: function () {
    test_basic_operation()
    //test_pre()

  }
};

