//
// Created by Fanzhe on 5/28/2017.
//

#include "modbus.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <errno.h>
using namespace std;


/**
 * Main Constructor of Modbus Connector Object
 * @param host IP Address of Host
 * @param port Port for the TCP Connection
 * @return     A Modbus Connector Object
 */
modbus::modbus(string host_, uint16_t port_ = 502) {
    HOST = host_;
    PORT = port_;
    _slaveid = 1;
    _msg_id = 1;
    _connected = false;

    if(HOST == "" || PORT == 0) {
            std::cout << "Missing Host and Port" << std::endl;
            //return false;
        } else {
            std::cout << "Found Proper Host "<< HOST << " and Port " <<PORT <<std::endl;
        }


      /*  int noBlock = 1;
        int res = ioctl(_socket,FIONBIO,(int)&noBlock);
        if(res == -1)
        {
        	 std::cout <<"Can`t set block regime" << std::endl;
        }*/

}


/**
 * Destructor of Modbus Connector Object
 */
modbus::~modbus(void) {
}


/**
 * Modbus Slave ID Setter
 * @param id  ID of the Modbus Server Slave
 */
void modbus::modbus_set_slave_id(int id) {
    _slaveid = id;
}



/**
 * Build up a Modbus/TCP Connection
 * @return   If A Connection Is Successfully Built
 */
bool modbus::modbus_connect()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket == -1)
    {
        std::cout <<"Error Opening Socket" <<std::endl;

    }else
    {
        std::cout <<"Socket Opened Successfully" << std::endl;
    }
    _server.sin_family = AF_INET;
    _server.sin_addr.s_addr = inet_addr(HOST.c_str());
    _server.sin_port = htons(PORT);

    int status = connect(_socket, (struct sockaddr*)&_server, sizeof(_server));
    if (status < 0)
    {
        printf( "That means: %s\n", strerror( errno ) );
        if(_connected == true)
        {
            modbus_close();
        }
        _connected = false;

        return false;
    }
    std::cout<< "Connected" <<std::endl;
    _connected = true;
    return true;
}


/**
 * Close the Modbus/TCP Connection
 */
void modbus::modbus_close() {
    close(_socket);
    std::cout <<"Socket Closed" <<std::endl;
}


/**
 * Modbus Request Builder
 * @param to_send   Message Buffer to Be Sent
 * @param address   Reference Address
 * @param func      Modbus Functional Code
 */
void modbus::modbus_build_request(uint8_t *to_send, int address, int func) {
    to_send[0] = (uint8_t) _msg_id >> 8;
    to_send[1] = (uint8_t) (_msg_id & 0x00FF);
    to_send[2] = 0;
    to_send[3] = 0;
    to_send[4] = 0;
    to_send[6] = (uint8_t) _slaveid;
    to_send[7] = (uint8_t) func;
    to_send[8] = (uint8_t) (address >> 8);
    to_send[9] = (uint8_t) (address & 0x00FF);
}


/**
 * Write Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of data to be Written
 * @param func      Modbus Functional Code
 * @param value     Data to Be Written
 */
ssize_t modbus::modbus_write(int address, int amount, int func, uint16_t *value) {
    if(func == WRITE_COIL || func == WRITE_REG) {
        uint8_t to_send[12];
        modbus_build_request(to_send, address, func);
        to_send[5] = 6;
        to_send[10] = (uint8_t) (value[0] >> 8);
        to_send[11] = (uint8_t) (value[0] & 0x00FF);
        return modbus_send(to_send, 12);
    } else if(func == WRITE_REGS){
        uint8_t to_send[13 + 2 * amount];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (5 + 2 * amount);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) (amount & 0x00FF);
        to_send[12] = (uint8_t) (2 * amount);
        for(int i = 0; i < amount; i++) {
            to_send[13 + 2 * i] = (uint8_t) (value[i] >> 8);
            to_send[14 + 2 * i] = (uint8_t) (value[i] & 0x00FF);
        }
        return modbus_send(to_send, 13 + 2 * amount);
    } else if(func == WRITE_COILS) {
        uint8_t to_send[14 + (amount -1) / 8 ];
        modbus_build_request(to_send, address, func);
        to_send[5] = (uint8_t) (7 + (amount -1) / 8);
        to_send[10] = (uint8_t) (amount >> 8);
        to_send[11] = (uint8_t) (amount >> 8);
        to_send[12] = (uint8_t) ((amount + 7) / 8);
        for(int i = 0; i < amount; i++) {
            to_send[13 + (i - 1) / 8] += (uint8_t) (value[i] << (i % 8));
        }
        return modbus_send(to_send, 14 + (amount - 1) / 8);
    }
}


/**
 * Read Request Builder and Sender
 * @param address   Reference Address
 * @param amount    Amount of Data to Read
 * @param func      Modbus Functional Code
 */
void modbus::modbus_read(int address, int amount, int func){
    uint8_t to_send[12];
    modbus_build_request(to_send, address, func);
    to_send[5] = 6;
    to_send[10] = (uint8_t) (amount >> 8);
    to_send[11] = (uint8_t) (amount & 0x00FF);
    modbus_send(to_send, 12);
}


/**
 * Read Holding Registers
 * MODBUS FUNCTION 0x03
 * @param address    Reference Address
 * @param amount     Amount of Registers to Read
 * @param buffer     Buffer to Store Data Read from Registers
 */
void modbus::modbus_read_holding_registers(int address, int amount, uint16_t *buffer) {
    if(_connected) {
        if(amount > 65535 || address > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        modbus_read(address, amount, READ_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try {
            modbus_error_handle(to_rec, READ_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
           }
        /*} catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Read Input Registers 
 * MODBUS FUNCTION 0x04
 * @param address     Reference Address
 * @param amount      Amount of Registers to Read
 * @param buffer      Buffer to Store Data Read from Registers
 */
void modbus::modbus_read_input_registers(int address, int amount, uint16_t *buffer) {
    if(_connected){
        if(amount > 65535 || address > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        modbus_read(address, amount, READ_INPUT_REGS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try {
            modbus_error_handle(to_rec, READ_INPUT_REGS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = ((uint16_t)to_rec[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) to_rec[10 + 2 * i];
            }
        /*} catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Read Coils           
 * MODBUS FUNCTION 0x01
 * @param address     Reference Address
 * @param amount      Amount of Coils to Read
 * @param buffer      Buffer to Store Data Read from Coils
 */
void modbus::modbus_read_coils(int address, int amount, bool *buffer) {
    if(_connected) {
        if(amount > 2040 || address > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        modbus_read(address, amount, READ_COILS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try {
            modbus_error_handle(to_rec, READ_COILS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
           }
        /*} catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Read Input Bits(Discrete Data)
 * MODBUS FUNCITON 0x02
 * @param address   Reference Address
 * @param amount    Amount of Bits to Read
 * @param buffer    Buffer to store Data Read from Input Bits
 */
void modbus::modbus_read_input_bits(int address, int amount, bool* buffer) {
    if(_connected) {
        if(amount > 2040 || address > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        modbus_read(address, amount, READ_INPUT_BITS);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try {
            modbus_error_handle(to_rec, READ_INPUT_BITS);
            for(int i = 0; i < amount; i++) {
                buffer[i] = (bool) ((to_rec[9 + i / 8] >> (i % 8)) & 1);
            }
        /*} catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Write Single Coils
 * MODBUS FUNCTION 0x05
 * @param address    Reference Address
 * @param to_write   Value to be Written to Coil
 */
void modbus::modbus_write_coil(int address, bool to_write) {
    if(_connected) {
        if(address > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        int value = to_write * 0xFF00;
        modbus_write(address, 1, WRITE_COIL, (uint16_t *)&value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try{
            modbus_error_handle(to_rec, WRITE_COIL);
        /*} catch (exception &e) {
            cout<<e.what()<<endl;
            delete(&to_rec);
            delete(&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Write Single Register
 * FUCTION 0x06
 * @param address   Reference Address
 * @param value     Value to Be Written to Register
 */
void modbus::modbus_write_register(int address, uint16_t value) {
    if(_connected) {
        if(address > 65535) {
            std::cout<<"modbus_amount_exception"<<std::endl;
        }
        ssize_t res = modbus_write(address, 1, WRITE_REG, &value);
//        if(res == -1 )
//        {
//            _connected = false;
//            printf( "That means2: %s\n", strerror( errno ) );
//            modbus_close();
//            return;
//        }
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try{
            modbus_error_handle(to_rec, WRITE_COIL);
        /*} catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Write Multiple Coils 
 * MODBUS FUNCTION 0x0F
 * @param address  Reference Address
 * @param amount   Amount of Coils to Write
 * @param value    Values to Be Written to Coils
 */
void modbus::modbus_write_coils(int address, int amount, bool *value) {
    if(_connected) {
        if(address > 65535 || amount > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        uint16_t temp[amount];
        for(int i = 0; i < 4; i++) {
            temp[i] = (uint16_t)value[i];
        }
        modbus_write(address, amount, WRITE_COILS,  temp);
        uint8_t to_rec[MAX_MSG_LENGTH];
        ssize_t k = modbus_receive(to_rec);
        //try{
            modbus_error_handle(to_rec, WRITE_COILS);
        /*} catch (exception &e) {
            cout << e.what() << endl;
            delete (&to_rec);
            delete (&k);
            throw e;
        }*/
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Write Multiple Registers 
 * MODBUS FUNCION 0x10
 * @param address Reference Address
 * @param amount  Amount of Value to Write
 * @param value   Values to Be Written to the Registers
 */
void modbus::modbus_write_registers(int address, int amount, uint16_t *value) {
    if(_connected) {
        if(address > 65535 || amount > 65535) {
        	std::cout<<"modbus_amount_exception"<<std::endl;
        }
        int size = modbus_write(address, amount, WRITE_REGS, value);
        uint8_t to_rec[MAX_MSG_LENGTH];
        //ssize_t k = modbus_receive(to_rec);
//        try{
//            modbus_error_handle(to_rec, WRITE_REGS);
//        } catch (exception &e) {
//            cout << e.what() << endl;
//            delete (&to_rec);
//            delete (&k);
//            throw e;
//        }
    } else {
    	std::cout<<"modbus_connect_exception"<<std::endl;
    }
}


/**
 * Data Sender
 * @param to_send Request to Be Sent to Server
 * @param length  Length of the Request
 * @return        Size of the request
 */
ssize_t modbus::modbus_send(uint8_t *to_send, int length) {
    _msg_id++;
    int addr = 0;


    return send(_socket, to_send, (size_t)length, addr);
}


/**
 * Data Receiver
 * @param buffer Buffer to Store the Data Retrieved
 * @return       Size of Incoming Data
 */
ssize_t modbus::modbus_receive(uint8_t *buffer) {
    return recv(_socket, (char *) buffer, 1024, 0);
}


/**
 * Error Code Handler
 * @param msg   Message Received from the Server
 * @param func  Modbus Functional Code
 */
void modbus::modbus_error_handle(uint8_t *msg, int func)
{
    if(msg[7] == func + 0x80)
    {
        switch(msg[8]){
        case EX_ILLEGAL_FUNCTION:
        {
            std::cout<< "modbus_illegal_function_exception"<<std::endl;
            break;
        }
        case EX_ILLEGAL_ADDRESS:
        {
            std::cout<< "modbus_illegal_address_exception"<<std::endl;
            break;
        }
        case EX_ILLEGAL_VALUE:
        {
            std::cout<< "modbus_illegal_data_value_exception"<<std::endl;
            break;
        }
        case EX_SERVER_FAILURE:
        {
            std::cout<< "modbus_server_failure_exception"<<std::endl;
            modbus_close();
            break;
        }
        case EX_ACKNOWLEDGE:
        {
            std::cout<< "modbus_acknowledge_exception"<<std::endl;
            modbus_close();
            break;
        }
        case EX_SERVER_BUSY:
        {
            std::cout<< "modbus_server_busy_exception"<<std::endl;
            modbus_close();
            break;
        }
        case EX_GATEWAY_PROBLEMP:
        case EX_GATEWYA_PROBLEMF:
        {
            std::cout<< "modbus_gateway_exception"<<std::endl;
            break;
        }
        default:
            break;
        }
    }
}
