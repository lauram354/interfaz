import React, { useState } from 'react';

export default function GamepadControl({ onCommand }) {
  const [activeButton, setActiveButton] = useState('stop');
  const [activeCommand, setActiveCommand] = useState('0');

  const handlePress = (cmd, name) => {
    // Solo envía el comando si es diferente al actual
    if (cmd !== activeCommand) {
      setActiveButton(name);
      setActiveCommand(cmd);
      onCommand(cmd);
      
      // Log para debugging
      console.log(`Comando enviado: ${cmd} (${name})`);
    }
  };

  return (
    <div className="gamepad-container">
      <div className="dpad-wrapper">
        {/* D-pad unificado tipo cruz */}
        <div className="dpad-unified">
          <div className="dpad-cross-unified">
            {/* Top */}
            <button
              onClick={() => handlePress('1', 'forward')}
              className={`dpad-button-unified top ${activeButton === 'forward' ? 'active' : ''}`}
              aria-label="forward"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Left */}
            <button
              onClick={() => handlePress('4', 'left')}
              className={`dpad-button-unified left ${activeButton === 'left' ? 'active' : ''}`}
              aria-label="left"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Center - STOP */}
            <button
              onClick={() => handlePress('0', 'stop')}
              className={`dpad-center-unified ${activeButton === 'stop' ? 'active' : ''}`}
              aria-label="stop"
            >
              <div className="center-inner-unified"></div>
            </button>

            {/* Right */}
            <button
              onClick={() => handlePress('3', 'right')}
              className={`dpad-button-unified right ${activeButton === 'right' ? 'active' : ''}`}
              aria-label="right"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Bottom */}
            <button
              onClick={() => handlePress('2', 'backward')}
              className={`dpad-button-unified bottom ${activeButton === 'backward' ? 'active' : ''}`}
              aria-label="backward"
            >
              <div className="button-triangle-unified"></div>
            </button>
          </div>
        </div>
      </div>

      <div className="control-info">
        <p className="info-text">Presiona para cambiar de dirección</p>
        <div className="command-display">
          {activeButton !== 'stop' ? (
            <span className="active-command">
              → {activeButton.toUpperCase()} ({activeCommand})
            </span>
          ) : (
            <span className="idle-command">DETENIDO (0)</span>
          )}
        </div>
      </div>
    </div>
  );
}