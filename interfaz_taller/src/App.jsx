import React, { useEffect, useState } from 'react';
import SensorCard from './components/SensorCard';
import GamepadControl from './components/GamepadControl';
import './App.css';

export default function App() {
  const [data, setData] = useState({ temperatura: 0, humedad: 0 });
  const [status, setStatus] = useState("Desconectado");
  const [isConnected, setIsConnected] = useState(false);

  // Cambia esta IP por la de tu ESP32
  const ESP_IP = "http://10.49.78.36";

  // Leer datos del ESP32 cada 3 segundos
  useEffect(() => {
    const fetchData = async () => {
      try {
        const res = await fetch(`${ESP_IP}/data`);
        if (!res.ok) throw new Error("Error al obtener datos");
        const json = await res.json();
        setData(json);
        setStatus("Conectado");
        setIsConnected(true);
      } catch (error) {
        console.error(error);
        setStatus("Sin conexión");
        setIsConnected(false);
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
    <div className="app-container">
      {/* Header compacto */}
      <header className="app-header">
        <div className="header-content">
          <div className="logo-section">
            <div className="rf-icon">📡</div>
            <div>
              <h1 className="title">FIRA CONTROL</h1>
            </div>
          </div>
          <div className="status-badge">
            <div className={`status-dot ${isConnected ? 'connected' : 'disconnected'}`}></div>
            <span className="status-text">{status}</span>
          </div>
        </div>
      </header>

      {/* Main content */}
      <main className="main-content">
        {/* Telemetry sidebar */}
        <aside className="telemetry-panel">
          <div className="sensors-grid">
            <SensorCard 
              label="Distancia" 
              value={data.temperatura} 
              unit="cm"
            />
            <SensorCard 
              label="Velocidad" 
              value={data.humedad} 
              unit="m/s"
            />
          </div>
        </aside>

        {/* Control panel */}
        <section className="control-panel">
          <div className="control-header">
          </div>
          <GamepadControl onCommand={sendCommand} />
        </section>
      </main>
    </div>
  );
}