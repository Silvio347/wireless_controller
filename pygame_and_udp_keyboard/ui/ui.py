def wait_udp_message(screen, font, prompt, udp_sock):
    from pygame import QUIT, event, display
    import pygame
    screen.fill((10, 10, 10))
    text = font.render(prompt, True, (255, 255, 255))
    screen.blit(text, text.get_rect(center=(screen.get_width() // 2, screen.get_height() // 2)))
    display.flip()

    while True:
        try:
            data, _ = udp_sock.recvfrom(1024)
            msg = data.decode("utf-8", errors="ignore").strip().lower()
            if msg:
                return msg
        except BlockingIOError:
            pass

        for e in event.get():
            if e.type == QUIT:
                pygame.quit()
                exit()
