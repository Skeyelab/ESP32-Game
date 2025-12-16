// Web server implementation

#include "web_server.h"
#include "../status/status_monitor.h"
#include "../games/game_manager.h"
#include <ArduinoJson.h>
#include <WiFi.h>

static WebServer* server = nullptr;
static bool serverRunning = false;

// HTML dashboard
static const char* html_dashboard = R"html(
<!DOCTYPE html>
<html>
<head>
    <title>ESP32 Game Status</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; margin: 20px; background: #1a1a1a; color: #fff; }
        .container { max-width: 600px; margin: 0 auto; }
        .card { background: #2a2a2a; padding: 20px; margin: 10px 0; border-radius: 8px; }
        .status { font-size: 24px; font-weight: bold; margin: 10px 0; }
        .score { font-size: 32px; color: #4CAF50; }
        .game-name { font-size: 18px; color: #2196F3; }
        .state { font-size: 16px; margin: 5px 0; }
        .input { display: inline-block; margin: 5px; padding: 5px 10px; border-radius: 4px; }
        .input.active { background: #4CAF50; }
        .input.inactive { background: #555; }
        button { background: #2196F3; color: white; border: none; padding: 10px 20px; border-radius: 4px; cursor: pointer; margin: 5px; }
        button:hover { background: #1976D2; }
        #refreshStatus { color: #4CAF50; font-size: 14px; margin-left: 10px; }
        .led-strip { display: flex; gap: 5px; margin: 20px 0; padding: 10px; background: #1a1a1a; border-radius: 8px; }
        .led-pixel { width: 40px; height: 40px; border-radius: 4px; border: 2px solid #333; transition: all 0.1s; }
        .led-label { font-size: 10px; text-align: center; color: #888; margin-top: 5px; }
        .game-selector { display: grid; grid-template-columns: repeat(auto-fill, minmax(120px, 1fr)); gap: 10px; margin: 10px 0; }
        .game-button { background: #333; color: white; border: 2px solid #555; padding: 10px; border-radius: 4px; cursor: pointer; text-align: center; }
        .game-button:hover { background: #444; border-color: #2196F3; }
        .game-button.active { background: #2196F3; border-color: #1976D2; }
        select { background: #333; color: white; border: 2px solid #555; padding: 10px; border-radius: 4px; width: 100%; font-size: 16px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>ESP32 Game Status</h1>
        <div class="card">
            <div class="game-name" id="gameName">Loading...</div>
            <div class="state" id="gameState">Loading...</div>
            <div class="score" id="score">0</div>
        </div>
        <div class="card">
            <h3>LED Strip Simulation</h3>
            <div class="led-strip" id="ledStrip">
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led0" style="background: #000;"></div>
                    <div class="led-label">0</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led1" style="background: #000;"></div>
                    <div class="led-label">1</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led2" style="background: #000;"></div>
                    <div class="led-label">2</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led3" style="background: #000;"></div>
                    <div class="led-label">3</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led4" style="background: #000;"></div>
                    <div class="led-label">4</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led5" style="background: #000;"></div>
                    <div class="led-label">5</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led6" style="background: #000;"></div>
                    <div class="led-label">6</div>
                </div>
                <div style="display: flex; flex-direction: column; align-items: center;">
                    <div class="led-pixel" id="led7" style="background: #000;"></div>
                    <div class="led-label">7</div>
                </div>
            </div>
        </div>
        <div class="card">
            <h3>Game Selection</h3>
            <div id="gameSelector" class="game-selector">
                <!-- Game buttons will be populated by JavaScript -->
            </div>
            <div style="margin-top: 10px;">
                <select id="gameSelect" onchange="selectGame(this.value)">
                    <option value="">Loading games...</option>
                </select>
            </div>
        </div>
        <div class="card">
            <h3>Input Status</h3>
            <div class="input" id="left">Left</div>
            <div class="input" id="right">Right</div>
            <div class="input" id="action">Action</div>
            <div class="input" id="alt">Alt</div>
        </div>
        <div class="card">
            <button onclick="refreshStatus()">Refresh Now</button>
            <span id="refreshStatus">Auto-refreshing every 500ms</span>
        </div>
    </div>
    <script>
        let autoRefreshInterval = null;

        function rgbToHex(r, g, b) {
            return '#' + [r, g, b].map(x => {
                const hex = x.toString(16);
                return hex.length === 1 ? '0' + hex : hex;
            }).join('');
        }

        function updateStatus(data) {
            document.getElementById('gameName').textContent = 'Game: ' + data.gameName;
            document.getElementById('score').textContent = 'Score: ' + data.score;

            const states = {
                '0': 'Playing',
                '1': 'Game Over',
                '2': 'Won',
                '3': 'Paused'
            };
            document.getElementById('gameState').textContent = 'State: ' + states[data.state];

            // Update LED strip
            if (data.leds && Array.isArray(data.leds)) {
                for (let i = 0; i < 8; i++) {
                    const led = document.getElementById('led' + i);
                    if (led) {
                        if (data.leds[i] && typeof data.leds[i] === 'object') {
                            const r = parseInt(data.leds[i].r) || 0;
                            const g = parseInt(data.leds[i].g) || 0;
                            const b = parseInt(data.leds[i].b) || 0;
                            led.style.background = rgbToHex(r, g, b);
                            // Add brightness effect
                            const brightness = Math.max(r, g, b);
                            led.style.boxShadow = brightness > 0 ? `0 0 ${brightness / 10}px ${rgbToHex(r, g, b)}` : 'none';
                        } else {
                            // Fallback if LED data is missing
                            led.style.background = '#000';
                            led.style.boxShadow = 'none';
                        }
                    }
                }
            }

            document.getElementById('left').className = 'input ' + (data.leftPressed ? 'active' : 'inactive');
            document.getElementById('right').className = 'input ' + (data.rightPressed ? 'active' : 'inactive');
            document.getElementById('action').className = 'input ' + (data.actionPressed ? 'active' : 'inactive');
            document.getElementById('alt').className = 'input ' + (data.altPressed ? 'active' : 'inactive');
        }

        let gamesList = [];
        let currentGameId = null;

        function loadGames() {
            fetch('/games')
                .then(response => response.json())
                .then(data => {
                    gamesList = data.games || [];
                    const selector = document.getElementById('gameSelector');
                    const select = document.getElementById('gameSelect');

                    selector.innerHTML = '';
                    select.innerHTML = '<option value="">Select a game...</option>';

                    gamesList.forEach(game => {
                        // Add button
                        const button = document.createElement('div');
                        button.className = 'game-button';
                        button.textContent = game.name;
                        button.onclick = () => selectGame(game.id);
                        button.id = 'gameBtn' + game.id;
                        selector.appendChild(button);

                        // Add option
                        const option = document.createElement('option');
                        option.value = game.id;
                        option.textContent = game.name;
                        select.appendChild(option);
                    });

                    // Load current game
                    loadCurrentGame();
                })
                .catch(err => console.error('Error loading games:', err));
        }

        function loadCurrentGame() {
            fetch('/game/current')
                .then(response => response.json())
                .then(data => {
                    currentGameId = data.gameId;
                    document.getElementById('gameSelect').value = data.gameId;
                    updateGameButtons();
                })
                .catch(err => console.error('Error loading current game:', err));
        }

        function updateGameButtons() {
            gamesList.forEach(game => {
                const button = document.getElementById('gameBtn' + game.id);
                if (button) {
                    if (game.id === currentGameId) {
                        button.classList.add('active');
                    } else {
                        button.classList.remove('active');
                    }
                }
            });
        }

        function selectGame(gameId) {
            if (gameId === '' || gameId === null) return;

            fetch('/game/select', {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ gameId: parseInt(gameId) })
            })
            .then(response => response.json())
            .then(data => {
                if (data.success) {
                    currentGameId = data.gameId;
                    document.getElementById('gameSelect').value = data.gameId;
                    updateGameButtons();
                    // Refresh status to show new game
                    refreshStatus();
                } else {
                    alert('Failed to switch game');
                }
            })
            .catch(err => {
                console.error('Error selecting game:', err);
                alert('Error switching game');
            });
        }

        function refreshStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => updateStatus(data))
                .catch(err => console.error('Error:', err));
        }

        function startAutoRefresh() {
            if (autoRefreshInterval) return;
            refreshStatus();
            // Refresh every 500ms for real-time updates
            autoRefreshInterval = setInterval(refreshStatus, 500);
        }

        // Start auto-refresh immediately on page load
        window.addEventListener('load', function() {
            loadGames();
            refreshStatus();
            startAutoRefresh();
        });
    </script>
</body>
</html>
)html";

// JSON status endpoint
void handleStatus() {
  GameStatus status = status_monitor_get();

  // Increased size to accommodate LED array (8 LEDs * ~20 bytes each = 160 bytes, plus other fields)
  StaticJsonDocument<768> doc;
  doc["gameName"] = status.gameName;
  doc["score"] = status.score;
  doc["state"] = status.state;
  doc["leftPressed"] = status.leftPressed;
  doc["rightPressed"] = status.rightPressed;
  doc["actionPressed"] = status.actionPressed;
  doc["altPressed"] = status.altPressed;
  doc["timestamp"] = status.timestamp;

  // Add LED array - ensure all 8 LEDs are included
  JsonArray ledsArray = doc.createNestedArray("leds");
  for (int i = 0; i < 8; i++) {
    JsonObject led = ledsArray.createNestedObject();
    led["r"] = (int)status.leds[i].r;
    led["g"] = (int)status.leds[i].g;
    led["b"] = (int)status.leds[i].b;
  }

  String response;
  serializeJson(doc, response);

  server->send(200, "application/json", response);
}

// Games list endpoint
void handleGames() {
  StaticJsonDocument<768> doc;
  JsonArray games = doc.createNestedArray("games");

  uint8_t numGames = game_manager_get_game_count();
  for (uint8_t i = 0; i < numGames; i++) {
    const GameInfo* info = game_manager_get_game_info(i);
    if (info) {
      JsonObject game = games.createNestedObject();
      game["id"] = info->id;
      game["name"] = info->name;
    }
  }

  String response;
  serializeJson(doc, response);

  server->send(200, "application/json", response);
}

// Current game endpoint
void handleGameCurrent() {
  StaticJsonDocument<256> doc;
  doc["gameId"] = game_manager_get_current_game();
  doc["gameName"] = game_manager_get_current_game_name();

  String response;
  serializeJson(doc, response);

  server->send(200, "application/json", response);
}

// Game selection endpoint (POST)
void handleGameSelect() {
  if (server->method() != HTTP_POST) {
    server->send(405, "text/plain", "Method Not Allowed");
    return;
  }

  String body = server->arg("plain");
  if (body.length() == 0) {
    server->send(400, "text/plain", "Bad Request: No JSON body");
    return;
  }

  StaticJsonDocument<256> doc;
  DeserializationError error = deserializeJson(doc, body);

  if (error) {
    server->send(400, "text/plain", "Bad Request: Invalid JSON");
    return;
  }

  if (!doc.containsKey("gameId")) {
    server->send(400, "text/plain", "Bad Request: Missing gameId");
    return;
  }

  uint8_t gameId = doc["gameId"];
  if (game_manager_set_game(gameId)) {
    StaticJsonDocument<256> responseDoc;
    responseDoc["success"] = true;
    responseDoc["gameId"] = gameId;
    responseDoc["gameName"] = game_manager_get_current_game_name();

    String response;
    serializeJson(responseDoc, response);
    server->send(200, "application/json", response);
  } else {
    server->send(400, "text/plain", "Bad Request: Invalid game ID");
  }
}

// Dashboard endpoint
void handleRoot() {
  server->send(200, "text/html", html_dashboard);
}

// 404 handler
void handleNotFound() {
  server->send(404, "text/plain", "Not Found");
}

void web_server_init() {
  if (server != nullptr) {
    return;
  }

  server = new WebServer(80);

  // Register handlers with explicit HTTP methods
  server->on("/", HTTP_GET, handleRoot);
  server->on("/status", HTTP_GET, handleStatus);
  server->on("/games", HTTP_GET, handleGames);
  server->on("/game/current", HTTP_GET, handleGameCurrent);
  server->on("/game/select", HTTP_POST, handleGameSelect);
  server->onNotFound(handleNotFound);

  server->begin();
  serverRunning = true;

  Serial.println("Web server started");
  Serial.print("Server IP: ");
  Serial.println(WiFi.localIP());
}

void web_server_update() {
  if (server != nullptr && serverRunning) {
    server->handleClient();
  }
}

bool web_server_is_running() {
  return serverRunning;
}

