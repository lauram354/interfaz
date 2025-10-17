import React from 'react';

export default function SensorCard({ label, value, unit, icon }) {
  return (
    <div className="sensor-card">
      <div className="sensor-header">
        <span className="sensor-icon">{icon}</span>
        <span className="sensor-label">{label}</span>
      </div>
      <div className="sensor-value">
        <span className="value-number">{value}</span>
        <span className="value-unit">{unit}</span>
      </div>
      <div className="sensor-indicator">
        <div className="indicator-bar"></div>
      </div>
    </div>
  );
}