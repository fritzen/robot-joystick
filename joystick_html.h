const char joystick_html[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta name='viewport' content='width=device-width, initial-scale=1.0, user-scalable=no' />
    <meta charset="utf-8" />
    <title>Robô Joystick</title>
    <style>

    #outerContainer {
        text-align: center;
    }

    #joyDiv {
        width: 200px;
        height: 200px;
        margin: auto;
    }


    @media (orientation: landscape) {
        #outerContainer {
            display: flex;
            justify-content: space-around;
            align-items: center;
            flex-wrap: wrap;
        }

        .button {
            margin: 10px;
        }
    }
    
        #container {
            width: 100%;
            height: 49vh;
            background-color: #333;
            display: flex;
            align-items: center;
            justify-content: center;
            overflow: hidden;
            border-radius: 7px;
            touch-action: none;
        }

        #item {
            width: 100px;
            height: 100px;
            background-color: rgb(245, 230, 99);
            border: 10px solid rgba(136, 136, 136, .5);
            border-radius: 50%;
            touch-action: none;
            user-select: none;
        }

        #item:active {
            background-color: rgba(168, 218, 220, 1.00);
        }

        #item:hover {
            cursor: pointer;
            border-width: 20px;
        }

        #area {
            position: fixed;
            right: 0;
            top: 0;
        }
    </style>

    <script type="text/javascript">
        var JoyStick = function (t, e) { var i = void 0 === (e = e || {}).title ? "joystick" : e.title, n = void 0 === e.width ? 0 : e.width, o = void 0 === e.height ? 0 : e.height, h = void 0 === e.internalFillColor ? "#00AA00" : e.internalFillColor, r = void 0 === e.internalLineWidth ? 2 : e.internalLineWidth, d = void 0 === e.internalStrokeColor ? "#003300" : e.internalStrokeColor, a = void 0 === e.externalLineWidth ? 2 : e.externalLineWidth, l = void 0 === e.externalStrokeColor ? "#008000" : e.externalStrokeColor, c = document.getElementById(t), u = document.createElement("canvas"); u.id = i, 0 == n && (n = c.clientWidth), 0 == o && (o = c.clientHeight), u.width = n, u.height = o, c.appendChild(u); var s = u.getContext("2d"), f = 0, v = 2 * Math.PI, g = (u.width - 110) / 2, w = g + 5, C = g + 30, m = u.width / 2, p = u.height / 2, L = u.width / 10, E = -1 * L, S = u.height / 10, k = -1 * S, W = m, G = p; function x() { s.beginPath(), s.arc(m, p, C, 0, v, !1), s.lineWidth = a, s.strokeStyle = l, s.stroke() } function y() { s.beginPath(), W < g && (W = w), W + g > u.width && (W = u.width - w), G < g && (G = w), G + g > u.height && (G = u.height - w), s.arc(W, G, g, 0, v, !1); var t = s.createRadialGradient(m, p, 5, m, p, 200); t.addColorStop(0, h), t.addColorStop(1, d), s.fillStyle = t, s.fill(), s.lineWidth = r, s.strokeStyle = d, s.stroke() } "ontouchstart" in document.documentElement ? (u.addEventListener("touchstart", function (t) { f = 1 }, !1), u.addEventListener("touchmove", function (t) { t.preventDefault(), 1 == f && (W = t.touches[0].pageX, G = t.touches[0].pageY, W -= u.offsetLeft, G -= u.offsetTop, s.clearRect(0, 0, u.width, u.height), x(), y()) }, !1), u.addEventListener("touchend", function (t) { f = 0, W = m, G = p, s.clearRect(0, 0, u.width, u.height), x(), y() }, !1)) : (u.addEventListener("mousedown", function (t) { f = 1 }, !1), u.addEventListener("mousemove", function (t) { 1 == f && (W = t.pageX, G = t.pageY, W -= u.offsetLeft, G -= u.offsetTop, s.clearRect(0, 0, u.width, u.height), x(), y()) }, !1), u.addEventListener("mouseup", function (t) { f = 0, W = m, G = p, s.clearRect(0, 0, u.width, u.height), x(), y() }, !1)), x(), y(), this.GetWidth = function () { return u.width }, this.GetHeight = function () { return u.height }, this.GetPosX = function () { return W }, this.GetPosY = function () { return G }, this.GetX = function () { return ((W - m) / w * 100).toFixed() }, this.GetY = function () { return ((G - p) / w * 100 * -1).toFixed() }, this.GetDir = function () { var t = "", e = W - m, i = G - p; return i >= k && i <= S && (t = "C"), i < k && (t = "N"), i > S && (t = "S"), e < E && ("C" == t ? t = "W" : t += "W"), e > L && ("C" == t ? t = "E" : t += "E"), t } };
    </script>
</head>

<body>

    <div id='outerContainer'>
        <div id="joyDiv"></div>
        <div id="ip"></p>
    </div>

<script>

        const view = document.getElementById('stream');
        const WS_URL = "ws://" + window.location.host + ":81";
        const ws = new WebSocket(WS_URL);

         document.getElementById("ip").innerHTML = WS_URL;

        ws.onmessage = message => {
            if (message.data instanceof Blob) {
                var urlObject = URL.createObjectURL(message.data);
                view.src = urlObject;
            }
        };

        var lastText, lastSend, sendTimeout;
        // limit sending to one message every 30 ms
        // https://github.com/neonious/lowjs_esp32_examples/blob/master/neonious_one/cellphone_controlled_rc_car/www/index.html
        function send(txt) {
            var now = new Date().getTime();
            if (lastSend === undefined || now - lastSend >= 30) {
                try {
                    ws.send(txt);
                    lastSend = new Date().getTime();
                    return;
                } catch (e) {
                    console.log(e);
                }
            }
            lastText = txt;
            if (!sendTimeout) {
                var ms = lastSend !== undefined ? 30 - (now - lastSend) : 30;
                if (ms < 0)
                    ms = 0;
                sendTimeout = setTimeout(() => {
                    sendTimeout = null;
                    send(lastText);
                }, ms);
            }
        }

        // Obtenha uma referência para o botão
        var botaoAbrirFechar = document.getElementById("botaoAbrirFechar");

        // Adicione um ouvinte de evento para o clique no botão
        botaoAbrirFechar.addEventListener("click", function(event) {
            // Evite o comportamento padrão do link
            event.preventDefault();
            
            // Chame a função que chama o método send("G")
            send("G");
        });

    </script>

</body>

<script type="text/javascript">
    // Create JoyStick object into the DIV 'joyDiv'
    var joy = new JoyStick('joyDiv');
    var inputPosX = document.getElementById("posizioneX");
    var inputPosY = document.getElementById("posizioneY");
    var direzione = document.getElementById("direzione");
    var fuerzaI = document.getElementById("fuerzaI");
    var fuerzaD = document.getElementById("fuerzaD");
    var x = document.getElementById("X");
    var y = document.getElementById("Y");

    //Steering control program, based on :
    // Differential Steering Joystick Algorithm
    //   by Calvin Hass
    //   https://www.impulseadventure.com/elec/

    function getfuerza(nJoyX, nJoyY) {
        var nMotMixL;
        var nMotMixR;
        var fPivYLimit = 32.0;
        var nMotPremixL;
        var nMotPremixR;
        var nPivSpeed;
        var fPivScale;

        if (nJoyY >= 0) {
            nMotPremixL = (nJoyX >= 0 ? 100.0 : 100.0 + parseFloat(nJoyX));
            nMotPremixR = (nJoyX >= 0 ? 100.0 - nJoyX : 100.0);
        } else {
            nMotPremixL = (nJoyX >= 0 ? 100.0 - nJoyX : 100.0);
            nMotPremixR = (nJoyX >= 0 ? 100.0 : 100.0 + parseFloat(nJoyX));
        }

        nMotPremixL = nMotPremixL * nJoyY / 100.0;
        nMotPremixR = nMotPremixR * nJoyY / 100.0;

        nPivSpeed = nJoyX;
        fPivScale = (Math.abs(nJoyY) > fPivYLimit) ? 0.0 : (1.0 - Math.abs(nJoyY) / fPivYLimit);

        nMotMixL = (1.0 - fPivScale) * nMotPremixL + fPivScale * (nPivSpeed);
        nMotMixR = (1.0 - fPivScale) * nMotPremixR + fPivScale * (-nPivSpeed);

        return Math.round(nMotMixL * 2.55) + "," + Math.round(nMotMixR * 2.55);
    }

    setInterval(function () { send(getfuerza(joy.GetX(), joy.GetY())); }, 300);

</script>

</html>
)=====";