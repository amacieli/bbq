#pragma once
#include <pgmspace.h>
static const char INDEX_HTML[] PROGMEM = R"HTML(<!doctype html>
<html><head><meta charset="utf-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>BBQ</title>
<style>body{font-family:sans-serif;text-align:center;background:#111;color:#eee}
#t{font-size:6rem;margin:.3em 0}button{font-size:2rem;margin:.3em;padding:.3em .8em}</style></head>
<body><div id="t">--</div><div>Setpoint: <span id="sp">--</span> &deg;C &nbsp; Fan: <span id="fan">--</span></div>
<button onclick="adj(-1)">&minus;</button><button onclick="adj(1)">+</button>
<script>
const es=new EventSource('/events');
es.addEventListener('state',e=>{const s=JSON.parse(e.data);
 t.textContent=s.temp.toFixed(1);sp.textContent=s.setpoint;fan.textContent=s.fan?'ON':'off';});
function adj(d){fetch('/api/setpoint?delta='+d,{method:'POST'});}
</script></body></html>)HTML";
