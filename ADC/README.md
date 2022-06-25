# ADC (Analog to Digital Converter)

연속적인 아날로그 신호를 0과 1로 구성된 디지털 신호로 변환하는 칩(I/C)이다. Sampling clock의 주기가 짧을수록(주파수가 높을수록) 원래 파형과 비슷하게 됨을 알 수 있다. <strong>반면 데이터의 양이 너무 많아져 처리하기 곤란할 수 있다는 단점이 되기도 한다.</strong> 초(second)당 몇 번 Sampling을 하는지를 SPS(sample per second)라고 하며, ADC의 가장 중요한 스펙 중 하나인 sampling rate의 단위이다. 

Sampling을 통해 측정한 전압의 값을 digital 값으로 바꾸는 과정이 Quantization이다. 이때 표현할 수 있는 값의 단위가 8-bit(0~255)면 256개로, 10-bit(0~1023)면 1024개로 더 정밀하게 표현된다. 

즉 ADC를 쓸 때 SPS와 resolution을 잘 확인하여 원하는 ADC를 선택해야 한다. SPS가 높으면 시간 축에서 정밀하게, Resolution이 높으면 전압 축에서 정밀하게 신호를 추출할 수 있다.