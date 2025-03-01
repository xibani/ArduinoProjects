import requests
import time
import subprocess

# Definir los esp con los sensores
ESP32_SENSOR_IP = "192.168.4.101"  # IP del ESP32
ESP01_SENSOR_IP = "192.168.4.102"  # IP del ESP-01S en el 


ERROR_THRESHOLD = 5  # Número de fallos consecutivos antes de reiniciar servicios

error_count = 0  # Contador de errores consecutivos

def restart_network_services():
    """ Reinicia hostapd y dnsmasq para intentar reestablecer la conexión. """
    print("\n🔄 ERROR: Se han producido 5 fallos consecutivos. Reiniciando servicios de red...")
    subprocess.run(["sudo", "systemctl", "restart", "hostapd"], check=False)
    subprocess.run(["sudo", "systemctl", "restart", "dnsmasq"], check=False)
    print("✅ Servicios de red reiniciados. Intentando reconectar...\n")

def get_sensor_data(ip, nombre):
    """ Solicita datos al ESP32 y reinicia los servicios si hay problemas repetidos. """
    global error_count  # Accedemos a la variable global

    try:
        print(f"📡 Solicitando datos a {nombre} ({ip})...")
        response = requests.get(f"http://{ip}/data", timeout=5)  # Timeout de 5 segundos
        print(f"✅ Respuesta de {nombre}: {response.status_code}")

        if response.status_code == 200:
            data = response.json()
            print(f"{nombre} - 🌡️ Temperatura: {data['temperatura']}°C, 💧 Humedad: {data['humedad']}%")
            #error_count = 0  # Reiniciar el contador si la conexión vuelve a funcionar
        else:
            print(f"❌ Error en la respuesta de {nombre}: {response.text}")
            error_count += 1

    except requests.exceptions.Timeout:
        print(f"⏳ ERROR: Tiempo de espera agotado para {nombre}")
        error_count += 1
    except requests.exceptions.ConnectionError:
        print(f"🔴 ERROR: No se pudo conectar a {nombre}. Contador de fallos: {error_count+1}")
        error_count += 1
    except Exception as e:
        print(f"⚠️ ERROR desconocido con {nombre}: {e}")
        error_count += 1

    # Si se alcanzan 5 errores consecutivos, reiniciar servicios
    if error_count >= ERROR_THRESHOLD:
        restart_network_services()
        error_count = 0  # Reiniciar el contador después del reinicio

while True:
    get_sensor_data(ESP32_SENSOR_IP, "ESP32 Sensor")
    get_sensor_data(ESP01_SENSOR_IP, "ESP01S Sensor")
    time.sleep(2)  # Espera 2 segundos antes de la siguiente solicitud
