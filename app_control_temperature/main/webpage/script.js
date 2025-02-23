
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


//MARK: SET LED RGB RED MAX - MIN
function setRed() {
    let min = document.getElementById('red-min').value;
    let max = document.getElementById('red-max').value;

    if (min === "" || max === "") {
        alert("Enter Min and Max values for RED");
        return;
    }

    // Crear el cuerpo de la solicitud con los datos configurados
    const data = {
        color: 'red',
        min: parseFloat(min),
        max: parseFloat(max)
    };

    // Enviar los datos al servidor mediante una solicitud POST
    fetch("/set_rgb", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
    .then(response => response.text())
    .then(data => alert(`Response: ${data}`))
    .catch(error => console.error("Error:", error));
}

//MARK: SET LED RGB GREEN MAX - MIN
function setGreen() {
    let min = document.getElementById('green-min').value;
    let max = document.getElementById('green-max').value;

    if (min === "" || max === "") {
        alert("Enter Min and Max values for GREEN");
        return;
    }

    // Crear el cuerpo de la solicitud con los datos configurados
    const data = {
        color: 'green',
        min: parseFloat(min),
        max: parseFloat(max)
    };

    // Enviar los datos al servidor mediante una solicitud POST
    fetch("/set_rgb", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
    .then(response => response.text())
    .then(data => alert(`Response: ${data}`))
    .catch(error => console.error("Error:", error));
}

//MARK: SET LED RGB BLUE MAX - MIN
function setBlue() {
    let min = document.getElementById('blue-min').value;
    let max = document.getElementById('blue-max').value;

    if (min === "" || max === "") {
        alert("Enter Min and Max values for BLUE");
        return;
    }

    // Crear el cuerpo de la solicitud con los datos configurados
    const data = {
        color: 'blue',
        min: parseFloat(min),
        max: parseFloat(max)
    };

    // Enviar los datos al servidor mediante una solicitud POST
    fetch("/set_rgb", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify(data)
    })
    .then(response => response.text())
    .then(data => alert(`Response: ${data}`))
    .catch(error => console.error("Error:", error));
}


//MARK: GET TEMPERATURE last reading
function getTemperature() {
    fetch("/turn_on_temp")
    .then(response => response.json())
    .then(data => {
        document.getElementById("temperature-display").innerText = `Temperature: ${data.Temperature} °C`;  // Usar la clave correcta del JSON
    })
    .catch(error => console.error("Error:", error));
}


//MARK: TOOGLE LED D2
function Toogle() {
    fetch("/toogle_led", {
        method: "POST",  // Cambiar a POST
        headers: { "Content-Type": "application/json" }
    })
    .then(response => response.text())
    .then(data => alert(data)) // Muestra una alerta con el mensaje de respuesta
    .catch(error => console.error("Error:", error));
}

//MARK: CHANGE COLOR ADC
function changeColor() {
    fetch("/change_color", {
        method: "POST",
        headers: { "Content-Type": "application/json" },
        body: JSON.stringify({ action: "change" })
    })
    .then(response => response.text())
    .then(data => alert(`Response: ${data}`))
    .catch(error => console.error("Error:", error));
}



//MARK: PRINT VALUE TEMPERATURE 
let tempInterval;
const maxEntries = 3;

function turnOnTemp() {
    console.log("turnOnTemp called");  // Log para verificar si la función se llama
    tempInterval = setInterval(() => {
        fetch("/turn_on_temp")
            .then(response => response.json())
            .then(data => {
                console.log("Temperature received:", data.Temperature);  // Log para verificar el valor recibido
                const tempOutput = document.getElementById("temperature-output");

                // Redondear a 1 decimal
                const roundedTemp = parseFloat(data.Temperature).toFixed(1);

                // Crear un nuevo elemento para la nueva temperatura
                const newTemp = document.createElement("div");
                newTemp.innerText = `Temperature: ${roundedTemp} °C`;

                // Añadir la nueva temperatura al final
                tempOutput.appendChild(newTemp);

                // Si hay más de 3 entradas, eliminar la más antigua
                while (tempOutput.childElementCount > maxEntries) {
                    tempOutput.removeChild(tempOutput.firstChild);
                }
            })
            .catch(error => console.error("Error:", error));
    }, 1000);
}

function turnOffTemp() {
    console.log("turnOffTemp called");  // Log para verificar si la función se llama
    clearInterval(tempInterval);
}


//MARK: PRINT VALUE ADC 
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
    clearInterval(adcInterval);
}

