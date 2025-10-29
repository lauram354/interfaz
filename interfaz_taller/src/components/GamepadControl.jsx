import React, { useState } from 'react';

export default function GamepadControl({ onCommand }) {
  const [activeButton, setActiveButton] = useState(null);

  const handlePress = (cmd, name) => {
    setActiveButton(name);
    onCommand(cmd);
  };

  const handleRelease = () => {
    setActiveButton(null);
    onCommand('0');
  };

  return (
    <div className="gamepad-container">
      <div className="dpad-wrapper">
        {/* D-pad unificado tipo cruz */}
        <div className="dpad-unified">
          <div className="dpad-cross-unified">
            {/* Top */}
            <button
              onMouseDown={() => handlePress('1', 'forward')}
              onMouseUp={handleRelease}
              onMouseLeave={handleRelease}
              onTouchStart={() => handlePress('1', 'forward')}
              onTouchEnd={handleRelease}
              className={`dpad-button-unified top ${activeButton === 'forward' ? 'active' : ''}`}
              aria-label="forward"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Left */}
            <button
              onMouseDown={() => handlePress('4', 'left')}
              onMouseUp={handleRelease}
              onMouseLeave={handleRelease}
              onTouchStart={() => handlePress('4', 'left')}
              onTouchEnd={handleRelease}
              className={`dpad-button-unified left ${activeButton === 'left' ? 'active' : ''}`}
              aria-label="left"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Center */}
            <button
              onMouseDown={() => handlePress('0', 'stop')}
              onMouseUp={handleRelease}
              className="dpad-center-unified"
              aria-label="stop"
            >
              <div className="center-inner-unified"></div>
            </button>

            {/* Right */}
            <button
              onMouseDown={() => handlePress('3', 'right')}
              onMouseUp={handleRelease}
              onMouseLeave={handleRelease}
              onTouchStart={() => handlePress('3', 'right')}
              onTouchEnd={handleRelease}
              className={`dpad-button-unified right ${activeButton === 'right' ? 'active' : ''}`}
              aria-label="right"
            >
              <div className="button-triangle-unified"></div>
            </button>

            {/* Bottom */}
            <button
              onMouseDown={() => handlePress('2', 'backward')}
              onMouseUp={handleRelease}
              onMouseLeave={handleRelease}
              onTouchStart={() => handlePress('2', 'backward')}
              onTouchEnd={handleRelease}
              className={`dpad-button-unified bottom ${activeButton === 'backward' ? 'active' : ''}`}
              aria-label="backward"
            >
              <div className="button-triangle-unified"></div>
            </button>
          </div>
        </div>

      </div>

      <div className="control-info">
        <p className="info-text">Mantén presionado para mover el robot</p>
        <div className="command-display">
          {activeButton ? (
            <span className="active-command">→ {activeButton.toUpperCase()}</span>
          ) : (
            <span className="idle-command">ESPERANDO...</span>
          )}
        </div>
      </div>
    </div>
  );
}