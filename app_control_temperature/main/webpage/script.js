
function connectWiFi() {
    let ssid = document.getElementById('wifi-ssid').value;
    let pass = document.getElementById('wifi-pass').value;

    fetch('/connect', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ ssid, pass })
    })
    .then(response => response.text())
    .then(data => alert(data))
    .catch(error => console.error('Error:', error));
}


// 🔹 Configurar Hotspot
function setHotspot() {
    let ssid = document.getElementById("hotspot-ssid").value;
    let password = document.getElementById("hotspot-pass").value;

    fetch("/set_hotspot", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ ssid, password }),
    })
    .then(response => response.json())
    .then(data => alert(data.message))
    .catch(error => console.error("Error:", error));
}

function setRGB(color) {
    let min = document.getElementById(`${color}-min`).value;
    let max = document.getElementById(`${color}-max`).value;

    if (min === "" || max === "") {
        alert(`Enter Min and Max values for ${color.toUpperCase()}`);
        return;
    }

    fetch(`/setRGB?color=${color}&min=${min}&max=${max}`)
    .then(response => response.text())
    .then(data => alert(data))
    .catch(error => console.error('Error:', error));
}

// 🔹 Obtener temperatura desde la ESP32
function getTemperature() {
    fetch("/get_temperature")
    .then(response => response.json())
    .then(data => {
        document.getElementById("temp-value").innerText = data.temperature;
    })
    .catch(error => console.error("Error:", error));
}

// Nueva función para encender el LED
function turnOnLED() {
    fetch("/led_on")
    .then(response => response.text())
    .then(data => alert(data)) // Muestra una alerta con el mensaje de respuesta
    .catch(error => console.error("Error:", error));
}

// Nueva función para encender el LED
function turnOffLED() {
    fetch("/led_off")
    .then(response => response.text())
    .then(data => alert(data)) // Muestra una alerta con el mensaje de respuesta
    .catch(error => console.error("Error:", error));
}

// 🔹 Encender/Apagar Print Temperature
function turnOnTemp() {
    fetch("/turn_on_temp")
    .then(response => response.json())
    .then(data => alert(data.message))
    .catch(error => console.error("Error:", error));
}

function turnOffTemp() {
    fetch("/turn_off_temp")
    .then(response => response.json())
    .then(data => alert(data.message))
    .catch(error => console.error("Error:", error));
}

// 🔹 Encender/Apagar Print ADC Pot
let adcInterval;

function turnOnADC() {
    adcInterval = setInterval(() => {
        fetch("/get_adc")
            .then(response => response.json())
            .then(data => {
                console.log("ADC Value received:", data.adc_value);  // Log para verificar el valor recibido
                document.getElementById("adc-value").innerText = `ADC Value: ${data.adc_value}`;
            })
            .catch(error => console.error("Error:", error));
    }, 1000);
}

function turnOffADC() {
    
    console.log("turnOffADC called");  // Log para verificar si la función se llama
    document.getElementById("adc-value").innerText = `ADC Value: --`;
    clearInterval(adcInterval);
}

