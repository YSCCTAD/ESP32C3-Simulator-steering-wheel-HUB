<h1>ESP32C3-模拟器方向盘HUB</h1>
<h1>简介</h1>
<p>由于合宙ESP32C3的引脚有限所以制作了12个按键加2个EC11编码器旋钮。同时因为本人还是初次接触ESP32C3和Arduino IDE，所以里面有部分代码是多余的（可以正常使用，只是多了一些没用的引脚定义，我使用Deepseek修改和完善代码导致）。代码存在某些引脚在开发版中不存在的多余引用，但是不影响该方向盘HUB的使用。各位如果可以完善并发布出来，十分感谢您的支持！</p>
<h3>准备工作</h3>
<ul>
  <li><a href="https://wiki.luatos.com/chips/esp32c3/board.html#id5">合宙ESP32C3资料</a></li>
  <li><a href="https://docs.arduino.cc/software/ide/">Arduino IDE下载</a></li><p>开发板管理地址（https://espressif.github.io/arduino-esp32/package_esp32_index.json）</p>
  <li><a href="https://github.com/lemmingDev/ESP32-BLE-Gamepad">ESP32-BLE-Gamepad库下载</a></li>
</ul><hr>
<h3>注意事项</h3>
<dl>
<dt>请选择开发板型号为</dt>
<dd>- 
  <code class="docutils literal notranslate"><span class="pre">AirM2M</span> <span class="pre">CORE</span> <span class="pre">ESP32C3</span></code></dd>
<dt>刷机烧录</dt>
<dd>- <code class="docutils literal notranslate"><span class="pre">日志波特率为921600</span></code></dd>
<dt>不可接的引脚</dt>
<dd> <code class="docutils literal notranslate"><span class="pre">GPIO09</span></code></dd></dd>
</dl><hr>
<h2>接线指引</h2>
<img width="1506" height="884" alt="ESP32-C3" src="https://github.com/user-attachments/assets/e3a42d51-30e9-48b9-9b69-2920d3452a3f" />


<img width="1778" height="3390" alt="20221023" src="https://github.com/user-attachments/assets/e5305552-c3c5-4e9c-ae15-1fe4a56d4c20" />
