#!/usr/bin/env python3.6
# Autor: Jakub Sadilek
# Login: xsadil07

import socket
import sys
import json


# Funkce prijma data z api a vrati je jako string.
def ReceiveData(sock):
    received = b''      # Pro data v bytech, ktere prijmame.
    while True:         # Prijmame dokud k nam chodi data.
        ret = sock.recv(1024)   # 1024 bytu - velikost bufferu.
        if not ret:     # Ukoncujici podminka, zadne data nedosly.
            break
        received += ret
    return received.decode('utf-8')     # Dekodujeme.


# Funkce osekne data o nepotrebne http info atd.. a vrati pouze JSON data.
def ConvToJson(inData):
    outData = inData.split("\r\n\r\n")[1]   # Odstranime hlavicku.
    try:
        outData = json.loads(outData)  # Vratime jako JSON.
    except Exception:   # Kontrola validnich json dat.
        sys.exit("JSON data corrupted.")
    return outData


# Vypise data podle zadani.
def PrintWeather(inData):
    WindSpeed = inData['wind']['speed'] * 3.6   # Konvertujeme na km/h
    try:    # Kontrola smeru vetru, pokud neni vitr, nemusi byt definovano.
        WindDeg = inData['wind']['deg']
    except Exception:
        WindDeg = "n/a"

    print(inData['name'] + "\n" +
          inData['weather'][0]['description'] + "\n" +
          "temp: " + str(inData['main']['temp']) + " °C\n" +
          "humidity: " + str(inData['main']['humidity']) + " %\n" +
          "pressure: " + str(inData['main']['pressure']) + " hPa\n" +
          "wind-speed: " + str(format(WindSpeed, '.2f')) + " km/h\n" +
          "wind-deg: " + str(WindDeg))


# Zkontrolujeme navratovy http kod a pripadne reagujeme na chybu.
def CheckErrCode(inData):
    code = int(inData.split(" ")[1])
    if code != 200:
        if code == 400:
            sys.exit("Error 400: Bad request.")
        elif code == 401:
            sys.exit("Error 401: Maybe bad API key.")
        elif code == 403:
            sys.exit("Error 403: Forbidden.")
        elif code == 404:
            sys.exit("Error 404: City not found.")
        elif code == 500:
            sys.exit("Error 500: Internal server error.")
        elif code == 502:
            sys.exit("Error 502: Bad gateway.")
        elif code == 503:
            sys.exit("Error 503: Service unavailable.")
        elif code == 504:
            sys.exit("Error 504: Gateway timeout.")
        elif code == 505:
            sys.exit("Error 505: HTTP version not supported.")
        else:
            sys.exit("Error {code}: Something is wrong.")

HOST = "api.openweathermap.org"     # Adresa hostitele.
PORT = 80           # HTTP port pres ktery se pripojime.

if (len(sys.argv) != 3 or not sys.argv[1] or not sys.argv[2]):
    sys.exit("Bad arguments. Use 'api_key=' and 'city=' and define.")

key = sys.argv[1]
city = sys.argv[2].lower()
request = f"GET /data/2.5/weather?q={city}&appid={key}&units=metric HTTP/1.0\r\n\r\n"   # Pozadavek na pocasi.

try:
    soc = socket.socket(socket.AF_INET, socket.SOCK_STREAM)  # Vytvorime socket.
    soc.connect((HOST, PORT))               # Pripojime se k API.
    soc.sendall(request.encode('utf-8'))    # Posleme pozadavek.
except socket.error:        # Kontrola jestli se vse povedlo.
    sys.exit("Socket error occurred.")
data = ReceiveData(soc)     # Prijmeme data.
soc.close()
CheckErrCode(data)          # Zkontrolujeme jestli jsme dostali nase data.
PrintWeather(ConvToJson(data))  # Vypiseme.
