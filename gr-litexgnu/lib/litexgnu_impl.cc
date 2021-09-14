/* -*- c++ -*- */
/*
 * Copyright 2021 gr-litexgnu author.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <inttypes.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <signal.h>
#include <gnuradio/io_signature.h>
#include "litexgnu_impl.h"

#include "litepcie.h"
#include "config.h"
#include "csr.h"
#include "flags.h"
#include "soc.h"


#include "liblitepcie.h"

static char litepcie_device[1024];
static int litepcie_device_num;

namespace gr {
  namespace litexgnu {
    using input_type = float;
    using output_type = float;
    litexgnu::sptr
    litexgnu::make()
    {
      return gnuradio::make_block_sptr<litexgnu_impl>(
        );
    }


static void info(void)
{
    int fd;
    int i;
    unsigned char fpga_identification[256];

    fd = open(litepcie_device, O_RDWR);
    if (fd < 0) {
        fprintf(stderr, "Could not init driver\n");
        exit(1);
    }

    for(i=0; i<256; i++)
        fpga_identification[i] = litepcie_readl(fd, CSR_IDENTIFIER_MEM_BASE + 4*i);
    printf("FPGA identification: %s\n", fpga_identification);
}

    /*
     * The private constructor
     */
    litexgnu_impl::litexgnu_impl()
      : gr::block("litexgnu",
              gr::io_signature::make(1, 1, sizeof(input_type)),
              gr::io_signature::make(1, 1, sizeof(output_type)))
    {}

    /*
     * Our virtual destructor.
     */
    litexgnu_impl::~litexgnu_impl()
    {
    }

    void
    litexgnu_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    litexgnu_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const float *in = (const float *)(input_items[0]);
      float *out = (float *)(output_items[0]);

      for(int i = 0; i < noutput_items; i++) {
        out[i] = in[i] * in[i];
      }

     const char *cmd;
     litepcie_device_num = 0;

     snprintf(litepcie_device, sizeof(litepcie_device), "/dev/litepcie%d", litepcie_device_num);
    
     info();     

      // Do <+signal processing+>
      // Tell runtime system how many input items we consumed on
      // each input stream.
      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace litexgnu */
} /* namespace gr */

