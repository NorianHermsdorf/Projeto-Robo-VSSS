# Robô VSSS - Firmware Base (ESP32-C3)

Este repositório contém o desenvolvimento do firmware para um robô de tração diferencial padrão VSSS. O projeto está focado na validação da camada de baixo nível e controle manual, servindo como fundação para a futura implementação de autonomia via **ESP-NOW**.

## Status do Projeto
Atualmente em fase de aquisição de hardware e prototipagem. O firmware valida a resposta dos atuadores, telemetria e o fail-safe do sistema.

## Hardware Previsto
* **Processador:** ESP32-C3 SuperMini
* **Atuadores:** Driver DRV8833 + 2x Motores N20 (6V 500RPM).
* **Energia:** Bateria LiPo 2S (8.4V) + Step-down Mini 560 Pro (5V).
* **Feedback:** LED RGB para status e divisor de tensão para monitoramento de carga.

## Funcionalidades do Firmware
* **Validação via BLE:** Controle inicial via Bluetooth LE (App Dabble) para testes de manobrabilidade e tração.
* **Cinemática Diferencial:** Mixagem de canais com curva exponencial para precisão em baixas velocidades.
* **Proteção de Hardware:** * Limitação de Duty Cycle (PWM) via software para proteger os motores de 6V contra a tensão da LiPo 2S.
    * Fail-safe: Corte automático de potência se a conexão cair por mais de 300ms.
* **Gestão de Bateria:** Monitoramento via ADC com sinalização visual (Blink) e detecção de carga.

## Próximos Passos
1. Finalização da montagem física.
2. **Migração para Autonomia (ESP-NOW):** Implementação da recepção de pacotes via protocolo ESP-NOW, permitindo o recebimento de comandos de decisão externa (Padrão IEEE VSSS) com latência ultra baixa e suporte a múltiplos robôs.
