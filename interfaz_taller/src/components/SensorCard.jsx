import React from 'react';

export default function SensorCard({ label, value }) {
  return (
    <div className="bg-gray-800 rounded-2xl shadow-lg p-6 w-64 text-center transition-transform hover:scale-105">
      <h2 className="text-xl font-semibold mb-2">{label}</h2>
      <p className="text-3xl font-bold text-emerald-400">{value}</p>
    </div>
  );
}
