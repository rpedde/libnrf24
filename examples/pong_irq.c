#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "rf24.h"

void send_pong(void * data)
{
  char buf[32];
  uint8_t i, len, done;
  rf24_t * radio = (rf24_t *) data;

  rf24_sync_status(radio);

  fprintf(stderr, "[rf24 pong callback] Got IRQ on pin %d. TX ok: %d, TX fail: %d RX ready: %d RX_LEN: %d PIPE: %d\n",
      radio->irq_pin,
      radio->status.tx_ok,
      radio->status.tx_fail_retries,
      radio->status.rx_data_available,
      radio->status.rx_data_len,
      radio->status.rx_data_pipe
      );

  if (radio->status.rx_data_available) {
    len = radio->status.rx_data_len;

    rf24_receive(radio, &buf, len);
    usleep(20);
    rf24_reset_status(radio);

    fprintf(stderr, "[rf24 pong callback] Data len: %d, data: %s\n", len, buf);

    rf24_stop_listening(radio);
    rf24_send(radio, &buf, 8 * sizeof(uint8_t));
    rf24_start_listening(radio);
  } else {
    rf24_reset_status(radio);
  }
}

int main(void)
{
  rf24_t radio;

  uint64_t pipe_addresses[2] = { 0xF0F0F0F0E1LL,  0xF0F0F0F0D2LL };
  uint8_t  pipe_no = 1;

  rf24_initialize(&radio, RF24_SPI_DEV_0, 25, 4);
  rf24_set_retries(&radio, 15, 15);

  rf24_open_writing_pipe(&radio, pipe_addresses[1]);
  rf24_open_reading_pipe(&radio, pipe_no, pipe_addresses[0]);

  rf24_start_listening(&radio);
  rf24_irq_poll(&radio, &send_pong);

  return 0;
}
// vim:ai:cin:et:sts=2 sw=2 ft=c
