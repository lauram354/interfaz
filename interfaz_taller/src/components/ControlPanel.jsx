import React from 'react';

export default function ControlPanel({ onCommand }) {
  const buttons = [
    { cmd: 'forward', label: '⬆️ Adelante' },
    { cmd: 'left', label: '⬅️ Izquierda' },
    { cmd: 'stop', label: '⏹️ Detener' },
    { cmd: 'right', label: '➡️ Derecha' },
    { cmd: 'backward', label: '⬇️ Atrás' },
  ];

  return (
    <div className="flex flex-wrap justify-center gap-4">
      {buttons.map((b) => (
        <button
          key={b.cmd}
          onClick={() => onCommand(b.cmd)}
          className="bg-blue-600 hover:bg-blue-700 text-white font-semibold px-4 py-2 rounded-lg transition"
        >
          {b.label}
        </button>
      ))}
    </div>
  );
}
