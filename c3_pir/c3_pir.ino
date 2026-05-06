#define PIR_PIN 4
#define LED_PIN 10
 

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);      
  
  // 初始LED熄灭
  digitalWrite(LED_PIN, LOW);
  
  Serial.println("PIR预热60秒...");
  delay(60000);
  Serial.println("预热完成，开始检测");
}

void loop() {
  int val = digitalRead(PIR_PIN);
  if(val == HIGH){
    digitalWrite(LED_PIN, HIGH);
    Serial.println("检测到人体移动");
  }else{
    digitalWrite(LED_PIN, LOW);
    Serial.println("无人");
  }
  delay(1000);
}