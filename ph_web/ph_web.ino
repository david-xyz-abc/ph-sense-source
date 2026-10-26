// pH sensor web UI for NodeMCU ESP8266
// Wiring: pH board O/P "+" -> A0, O/P "-" -> GND, pH board on its own 9V supply
// WiFi credentials go in secrets.h (copy secrets.h.example)
// Open http://ph.local or the IP printed on serial

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include "secrets.h"

// Uncalibrated estimate: assumes output is linear 500mV (pH 0) .. 3000mV (pH 14).
// Replace with two-point calibration from buffer solutions for real values.
const float MV_AT_PH0 = 500.0;
const float MV_AT_PH14 = 3000.0;
const float VREF_MV = 3300.0;

ESP8266WebServer server(80);

const char PAGE[] PROGMEM = R"html(<!doctype html>
<html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>pH Monitor</title>
<style>
:root{--bg:#f6f7f9;--card:#fff;--fg:#1c1e21;--muted:#6b7280;--line:#2563eb;--grid:#e5e7eb}
@media(prefers-color-scheme:dark){:root{--bg:#111318;--card:#1b1e25;--fg:#e8eaed;--muted:#9aa0a6;--line:#60a5fa;--grid:#2a2e37}}
body{margin:0;font-family:system-ui,sans-serif;background:var(--bg);color:var(--fg);display:flex;justify-content:center;padding:16px}
.card{background:var(--card);border-radius:14px;padding:20px;width:100%;max-width:520px;box-shadow:0 1px 3px rgba(0,0,0,.08)}
h1{font-size:18px;margin:0 0 12px}
.ph{font-size:64px;font-weight:700;line-height:1;font-variant-numeric:tabular-nums}
.sub{color:var(--muted);font-size:14px;margin-top:6px}
.bar{height:12px;border-radius:6px;margin:18px 0 4px;position:relative;
 background:linear-gradient(90deg,#dc2626,#f97316,#eab308,#22c55e,#14b8a6,#3b82f6,#7c3aed)}
.mark{position:absolute;top:-4px;width:4px;height:20px;background:var(--fg);border-radius:2px;transition:left .3s}
.scale{display:flex;justify-content:space-between;color:var(--muted);font-size:12px}
canvas{width:100%;height:140px;margin-top:18px}
.note{color:var(--muted);font-size:12px;margin-top:10px}
.off{opacity:.4}
</style></head><body><div class="card" id="card">
<h1>pH Monitor</h1>
<div class="ph" id="ph">--</div>
<div class="sub"><span id="mv">-- mV</span> &middot; <span id="label">waiting&hellip;</span></div>
<div class="bar"><div class="mark" id="mark" style="left:50%"></div></div>
<div class="scale"><span>0 acidic</span><span>7 neutral</span><span>14 alkaline</span></div>
<canvas id="c"></canvas>
<div class="note">Uncalibrated estimate. Calibrate with buffer solutions for accurate pH.</div>
</div><script>
const hist=[],MAX=120,c=document.getElementById('c'),x=c.getContext('2d');
function label(p){return p<6.5?'acidic':p>7.5?'alkaline':'neutral'}
function draw(){
 const w=c.width=c.clientWidth*devicePixelRatio,h=c.height=c.clientHeight*devicePixelRatio,s=getComputedStyle(document.body);
 x.strokeStyle=s.getPropertyValue('--grid');x.lineWidth=1;
 for(let p=0;p<=14;p+=7){const y=h-p/14*h;x.beginPath();x.moveTo(0,y);x.lineTo(w,y);x.stroke()}
 if(hist.length<2)return;
 x.strokeStyle=s.getPropertyValue('--line');x.lineWidth=2*devicePixelRatio;x.beginPath();
 hist.forEach((p,i)=>{const X=i/(MAX-1)*w,Y=h-Math.min(14,Math.max(0,p))/14*h;i?x.lineTo(X,Y):x.moveTo(X,Y)});x.stroke()}
async function tick(){
 try{const d=await (await fetch('/data')).json();
  document.getElementById('card').classList.remove('off');
  document.getElementById('ph').textContent=d.ph.toFixed(2);
  document.getElementById('mv').textContent=d.mv+' mV';
  document.getElementById('label').textContent=label(d.ph);
  document.getElementById('mark').style.left=Math.min(100,Math.max(0,d.ph/14*100))+'%';
  hist.push(d.ph);if(hist.length>MAX)hist.shift();draw();
 }catch(e){document.getElementById('card').classList.add('off');document.getElementById('label').textContent='disconnected'}}
setInterval(tick,1000);tick();addEventListener('resize',draw);
</script></body></html>)html";

int readMillivolts() {
  long sum = 0;
  for (int i = 0; i < 50; i++) {
    sum += analogRead(A0);
    delay(2);
  }
  return (sum / 50) * VREF_MV / 1023;
}

void handleData() {
  int mv = readMillivolts();
  float ph = (mv - MV_AT_PH0) * 14.0 / (MV_AT_PH14 - MV_AT_PH0);
  server.send(200, "application/json",
              String("{\"mv\":") + mv + ",\"ph\":" + String(ph, 2) + "}");
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("\nConnecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.printf("\nConnected! Open http://%s or http://ph.local\n",
                WiFi.localIP().toString().c_str());

  MDNS.begin("ph");
  server.on("/", [] { server.send_P(200, "text/html", PAGE); });
  server.on("/data", handleData);
  server.begin();
}

void loop() {
  server.handleClient();
  MDNS.update();
}
