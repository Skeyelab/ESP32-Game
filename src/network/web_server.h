// Web server for status monitoring

#ifndef WEB_SERVER_H
#define WEB_SERVER_H

#include <Arduino.h>
#include <WebServer.h>

// Initialize web server
void web_server_init();

// Update (call in loop)
void web_server_update();

// Check if server is running
bool web_server_is_running();

#endif // WEB_SERVER_H

