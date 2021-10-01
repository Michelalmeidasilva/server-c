#define BUFFER_SIZE 1024
#define CONNECTION_NUMBER 5
#define QUEUE_PENDING_CONNECTION 10
#define QUEUE_SIZE 10
#define LIMIT_CLIENTS 5

#define VERSION_HTTP1_0 "HTTP/1.0"
#define VERSION_HTTP1_1 "HTTP/1.1"

#define HTML_TYPE "html"
#define JPEG_TYPE "jpeg"
#define PNG_TYPE "png"

typedef struct {
  char * method;
  char * resource;
  char * protocol;
} Request;
