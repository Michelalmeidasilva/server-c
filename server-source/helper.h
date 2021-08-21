struct configs {
    char * port_number;
    char * server_path;
};


char * get_current_path();
struct configs read_env(char * variable_name);

