void setup() {
  Serial1.begin(9600);
}

void loop() {
  Serial1.write("hoi");
  delay(100);  
}
