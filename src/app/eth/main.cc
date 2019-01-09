/*
 * \brief  Test client for the Hello RPC interface
 * \author Björn Döbel
 * \author Norman Feske
 * \date   2008-03-20
 */

/*
 * Copyright (C) 2008-2017 Genode Labs GmbH
 *
 * This file is part of the Genode OS framework, which is distributed
 * under the terms of the GNU Affero General Public License version 3.
 */

#include <base/component.h>
#include <base/log.h>
#include <libc/component.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <util/string.h>

void Libc::Component::construct(Libc::Env &env)
{
  Libc::with_libc([&] () {
		    Genode::log("hello test completed");

		    struct sockaddr_in srv_addr;
		    srv_addr.sin_port = htons(8080);
		    srv_addr.sin_family = AF_INET;
		    srv_addr.sin_addr.s_addr = inet_addr("131.159.12.17");

		    int sd = ::socket(AF_INET, SOCK_STREAM, 0);
		    ::connect(sd, (struct sockaddr *)&srv_addr, sizeof(srv_addr));

		    char buffer[16384];
		    ::recv(sd, buffer, 16383, 0);
		    Genode::String<16383> s(buffer);
		    Genode::log(s);

		    // ----------------------------------

		  });
}
