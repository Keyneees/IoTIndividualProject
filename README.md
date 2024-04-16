# IoT Individual Project - Signal Sampling
Signal s as input => 1 or more sinusoid with noises

1) oversample -> š
2) fft(š) -> max
3) 2*max -> S
4) f(S) -> M
5) communicate M over MQTT

  The first three points require <b>energy saving estimation</b>
