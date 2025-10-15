import React, { useEffect, useState } from 'react';
import SensorCard from './components/SensorCard';
import ControlPanel from './components/ControlPanel';

export default function App() {
  const [data, setData] = useState({ temperatura: 0, humedad: 0 });
  const [status, setStatus] = useState("Desconectado");

  // Cambia esta IP por la de tu ESP32
  const ESP_IP = "http://192.168.0.4";

  // Leer datos del ESP32 cada 3 segundos
  useEffect(() => {
    const fetchData = async () => {
      try {
        const res = await fetch(`${ESP_IP}/data`);
        if (!res.ok) throw new Error("Error al obtener datos");
        const json = await res.json();
        setData(json);
        setStatus("Conectado ✅");
      } catch (error) {
        console.error(error);
        setStatus("Sin conexión ❌");
      }
    };

    fetchData();
    const interval = setInterval(fetchData, 3000);
    return () => clearInterval(interval);
  }, []);

  // Función para enviar comandos
  const sendCommand = async (cmd) => {
    try {
      const res = await fetch(`${ESP_IP}/move?cmd=${cmd}`);
      const result = await res.json();
      console.log("Comando enviado:", cmd, result);
    } catch (error) {
      console.error("Error enviando comando:", error);
    }
  };

  return (
    <div className="min-h-screen bg-gray-900 text-white flex flex-col items-center justify-center p-6">
      <h1 className="text-3xl font-bold mb-4">🌐 Monitor de Sensores ESP32</h1>
      <p className="mb-4">{status}</p>

      <div className="grid grid-cols-1 sm:grid-cols-2 gap-6 mb-8">
        <SensorCard label="Temperatura" value={`${data.temperatura.toFixed(2)} °C`} />
        <SensorCard label="Humedad" value={`${data.humedad.toFixed(2)} %`} />
      </div>

      <ControlPanel onCommand={sendCommand} />
    </div>
  );
}
