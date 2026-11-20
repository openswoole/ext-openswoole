# llhttp

Bundled llhttp 9.3.0 (https://github.com/nodejs/llhttp), MIT licensed, see LICENSE.

Local change, keep it when updating:

- `llhttp.c`: HTAB is accepted as whitespace after the final transfer coding, so the parser
  agrees with the server's own `Transfer-Encoding` check (search for "includes HTAB").
