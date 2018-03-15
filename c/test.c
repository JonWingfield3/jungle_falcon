int main(void) {
volatile  int a = 0x55;
volatile  int b = 0x44;
volatile  int c = a + b;
  
  for(int ii = 0; ii < 100; ++ii) {
    a <<= 1;
    b >>= 1;
    c = a | b;
  }

  while(1){}   
  return 0;
}
