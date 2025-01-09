#pragma once

// Sleep for given ticks
void ksleep(int ticks);

struct interrupt_frame;

// Trap gate
void trap(struct interrupt_frame* frame);
