#include <WiFi.h>

WiFiServer server(80);
// AP 用のSSID/パスワード（必要に応じて変更してください）
const char* ssid = "ESP32_hoge"; // APのSSIDを入力
const char* password = "12345678"; // APのパスワードを入力

// 内部状態
unsigned long hoge1 = 100;
unsigned long hoge2 = 100;
unsigned long hoge3 = 100;

String webpage = "";

// Build the simple web page (slider + form)
// シンプルなHTMLページを作る（GETクエリで設定します）
String buildPage() {
  String p = "<!doctype html><html><head><meta name=\"viewport\" content=\"width=device-width,initial-scale=1\">";
  p += "<title>PID設定</title>";
  p += "</head><body style=\"font-family:Arial;margin:12px;\">";
  p += "<h3>LEDの点灯/消灯時間 (ミリ秒)</h3>";
  p += "<p>現在の 比例: " + String(hoge1) + "</p>";
  p += "<p>現在の 微分: " + String(hoge2) + "</p>";
  p += "<p>現在の 積分: " + String(hoge3) + "</p>";
  p += "<form action=\"/\" method=\"GET\">";
  p += "比例: <input name=\"hoge1\" value=\"" + String(hoge1) + "\"><br><br>";
  p += "微分: <input name=\"hoge2\" value=\"" + String(hoge2) + "\"><br><br>";
  p += "積分: <input name=\"hoge3\" value=\"" + String(hoge3) + "\"><br><br>";
  p += "<button type=\"submit\">Apply</button>";
  p += "</form>";
  p += "</body></html>";
  return p;
}

// ここではWebサーバを使わず、WiFiServerで手動でHTTPリクエストを処理します
void handleClient() {
  WiFiClient client = server.available();
  if (!client) return; // クライアントがいなければ何もしない

  // --- クライアント接続 ---
  Serial.println("[handleClient] クライアント接続あり");

  // クライアントからのリクエストを読み取る（1行目だけ使う）
  String req = client.readStringUntil('\r'); // 例: GET /?on=1000&off=500 HTTP/1.1
  client.readStringUntil('\n'); // 残りのライン破棄
  Serial.print("[handleClient] リクエスト行: "); Serial.println(req);

  // 例: GET /?on=1000&off=500 HTTP/1.1
  int idx = req.indexOf(' '); // 一番最初に現れる空白の位置
  int idx2 = req.indexOf(' ', idx + 1); // idx+1以上で最初に現れる（=2つ目の）空白の位置
  String path = req.substring(idx + 1, idx2); // 例: /?on=1000&off=500
  Serial.print("[handleClient] パス解析: "); Serial.println(path);

  int hoge1Pos = path.indexOf("hoge1="); // "on="の位置
  if (hoge1Pos >= 0) {
    int amp = path.indexOf('&', hoge1Pos); // &の位置
    String onVal;
    if (amp >= 0) onVal = path.substring(hoge1Pos + 6, amp); // &があれば、"on="の後から&までを"onVal"に
    else {
      int q = path.indexOf(' ', hoge1Pos); // 次の空白の位置
      onVal = path.substring(hoge1Pos + 6, q < 0 ? path.length() : q); // 空白があれば、"on="の後から空白まで、なければそれ以降全部を"onVal"に
    }
    Serial.print("[handleClient] on の文字列: "); Serial.println(onVal);
    unsigned long v = onVal.toInt(); // 数値変換
    
    hoge1 = v; // 設定反映
    Serial.print("[handleClient] ledOnMs を設定: "); Serial.println(onVal);
  }

  int hoge2Pos = path.indexOf("hoge2="); // "on="の位置
  if (hoge2Pos >= 0) {
    int amp = path.indexOf('&', hoge2Pos); // &の位置
    String onVal;
    if (amp >= 0) onVal = path.substring(hoge2Pos + 6, amp); // &があれば、"on="の後から&までを"onVal"に
    else {
      int q = path.indexOf(' ', hoge2Pos); // 次の空白の位置
      onVal = path.substring(hoge2Pos + 6, q < 0 ? path.length() : q); // 空白があれば、"on="の後から空白まで、なければそれ以降全部を"onVal"に
    }
    Serial.print("[handleClient] on の文字列: "); Serial.println(onVal);
    unsigned long v = onVal.toInt(); // 数値変換
    
    hoge2 = v; // 設定反映
    Serial.print("[handleClient] ledOnMs を設定: "); Serial.println(onVal);
  }

  int hoge3Pos = path.indexOf("hoge3="); // "on="の位置
  if (hoge3Pos >= 0) {
    int amp = path.indexOf('&', hoge3Pos); // &の位置
    String onVal;
    if (amp >= 0) onVal = path.substring(hoge3Pos + 6, amp); // &があれば、"on="の後から&までを"onVal"に
    else {
      int q = path.indexOf(' ', hoge3Pos); // 次の空白の位置
      onVal = path.substring(hoge3Pos + 6, q < 0 ? path.length() : q); // 空白があれば、"on="の後から空白まで、なければそれ以降全部を"onVal"に
    }
    Serial.print("[handleClient] on の文字列: "); Serial.println(onVal);
    unsigned long v = onVal.toInt(); // 数値変換
    
    hoge3 = v; // 設定反映
    Serial.print("[handleClient] ledOnMs を設定: "); Serial.println(onVal);
  }


  // クライアントに簡単なHTMLを返す
  String page = buildPage(); // 最新の状態でページを作成
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.print("Content-Length: "); client.println(page.length());
  client.println();
  client.print(page);
  Serial.print("[handleClient] 応答を送信 (長さ="); Serial.print(page.length()); Serial.println(")");

  delay(1);
  client.stop();
  Serial.println("[handleClient] クライアント切断");
}

void setup() {

  // シリアル開始（デバッグ用）
  Serial.begin(115200);

  Serial.print("Starting AP: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(ssid, password);
  if (!ok) {
    Serial.println("softAP failed");
    return;
  }

  IPAddress ip = WiFi.softAPIP();
  Serial.println("WiFi AP started");
  Serial.print("AP IP Address : ");
  Serial.println(ip);

  server.begin();
  Serial.println("Server started (AP mode)");

  webpage = buildPage();
}

void loop() {
  // クライアント処理（HTTPリクエストを読み取り、設定があれば反映）
  handleClient();

  Serial.println(hoge1);
  Serial.println(hoge2);
  Serial.println(hoge3);
  delay(1000);
}
