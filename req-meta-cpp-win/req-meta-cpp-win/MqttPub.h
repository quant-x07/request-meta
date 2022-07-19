#pragma once
/*
 * This example shows how to publish messages from outside of the Mosquitto network loop.
 */
//#include <Winsock2.h>
#include <mosquitto.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 //#include <unistd.h>
#include <thread>         // std::thread
#include <mutex>          // std::mutex
#include <chrono>
#include <iostream>

extern std::mutex g_display_mutex;

/*DWORD WINAPI*/ void ThreadProc(void *);

class MqttPub
{
public:
    MqttPub() {
		server = "121.4.14.59";
		port = 1883;
		clientid = "test";
		password = "tset";
    }
    static MqttPub* instance();

    /* Callback called when the client receives a CONNACK message from the broker. */
    static void on_connect(struct mosquitto *mosq, void *obj, int reason_code)
    {
		MqttPub *c = static_cast<MqttPub*>(obj);
        /* Print out the connection result. mosquitto_connack_string() produces an
         * appropriate string for MQTT v3.x clients, the equivalent for MQTT v5.0
         * clients is mosquitto_reason_string().
         */
        printf("on_connect: %s\n", mosquitto_connack_string(reason_code));
        if (reason_code != 0) {
            /* If the connection fails for any reason, we don't want to keep on
             * retrying in this example, so disconnect. Without this, the client
             * will attempt to reconnect. */
            mosquitto_disconnect(mosq);
            return;
        }

        /* You may wish to set a flag here to indicate to your application that the
         * client is now connected. */
		int rc;

		std::string topic = "";
		topic += "c/" + c->clientid + "/#";
		rc = mosquitto_subscribe(mosq, NULL, topic.c_str(), 0);
		if (rc) {
			fprintf(stderr, "MQTT - Subscribe ERROR: %s\n", mosquitto_strerror(rc));
			return;
		}
		printf("sub rc %d\n", rc);
    }


    /* Callback called when the client knows to the best of its abilities that a
     * PUBLISH has been successfully sent. For QoS 0 this means the message has
     * been completely written to the operating system. For QoS 1 this means we
     * have received a PUBACK from the broker. For QoS 2 this means we have
     * received a PUBCOMP from the broker. */
    static void on_publish(struct mosquitto *mosq, void *obj, int mid)
    {
        //printf("Message with mid %d has been published.\n", mid);
    }

    static void on_disconnect(struct mosquitto *m, void *obj, int rc)
    {
        MqttPub *c = static_cast<MqttPub*>(obj);
        printf("Disconnection code: %d\n", rc);
        if (0 == rc) {
            c->_conn_lost = false;
            printf("Successfully disconnected from %s (port %d).\n", c->server.c_str(), c->port);
        }
        else {
            c->_conn_lost = true;
            printf("Disconnected from %s (port %d).\n", c->server.c_str(), c->port);
        }
    }

    static void on_subscribe(struct mosquitto *m, void *obj, int mid, int qos_count, const int *granted_qos)
    {
        printf("Packet ID %d: The topic has been subscribed.\n", mid);
    }

    static void on_message(struct mosquitto *, void *, const struct mosquitto_message *msg);

    static void on_unsubscribe(struct mosquitto *m, void *obj, int mid)
    {
        printf("Packet ID %d: The topic has been unsubscribed.\n", mid);
    }

    int client_init()
    {
        int rc;

        /* Required before calling other mosquitto functions */
        mosquitto_lib_init();

        /* Create a new client instance.
         * id = NULL -> ask the broker to generate a client id for us
         * clean session = true -> the broker should remove old sessions when we connect
         * obj = NULL -> we aren't passing any of our private data for callbacks
         */
        mosq = mosquitto_new(NULL, true, this);
        if (mosq == NULL) {
            fprintf(stderr, "Error: Out of memory.\n");
            return 1;
        }
        mosquitto_username_pw_set(mosq, clientid.c_str(), password.c_str());

        /* Configure callbacks. This should be done before connecting ideally. */
        mosquitto_connect_callback_set(mosq, MqttPub::on_connect);
        mosquitto_publish_callback_set(mosq, MqttPub::on_publish);
        mosquitto_disconnect_callback_set(mosq, MqttPub::on_disconnect);
        mosquitto_subscribe_callback_set(mosq, MqttPub::on_subscribe);
        mosquitto_message_callback_set(mosq, MqttPub::on_message);
        mosquitto_unsubscribe_callback_set(mosq, MqttPub::on_unsubscribe);
        mosquitto_threaded_set(mosq, true);

        /* Connect to test.mosquitto.org on port 1883, with a keepalive of 60 seconds.
         * This call makes the socket connection only, it does not complete the MQTT
         * CONNECT/CONNACK flow, you should use mosquitto_loop_start() or
         * mosquitto_loop_forever() for processing net traffic. */

        rc = mosquitto_connect(mosq, server.c_str(), port, 60);
        if (rc != MOSQ_ERR_SUCCESS) {
            mosquitto_destroy(mosq);
            fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
            return 1;
        }

        /* Run the network loop in a background thread, this call returns quickly. */
        //rc = mosquitto_loop_forever(mosq, 1000, 1);
        //if (rc != MOSQ_ERR_SUCCESS) {
        //    mosquitto_destroy(mosq);
        //    fprintf(stderr, "Error: %s\n", mosquitto_strerror(rc));
        //    return 1;
        //}

        // hThread1 = CreateThread(NULL, 0, ThreadProc, this, 0, NULL);
        hThread1 = new std::thread(ThreadProc, this);
        //if (!hThread1) {
        //    printf("Failed to create the MQTT thread.\n");
        //    abort();
        //    return -3;
        //}


        /* At this point the client is connected to the network socket, but may not
         * have completed CONNECT/CONNACK.
         * It is fairly safe to start queuing messages at this point, but if you
         * want to be really sure you should wait until after a successful call to
         * the connect callback.
         * In this case we know it is 1 second before we start publishing.
         */

         // NOTE: test
         //while (1) {
         //    publish_sensor_data(mosq);
         //}

         // mosquitto_lib_cleanup();
        return 0;
    }
    void client_set_running_status(bool status)
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _run = status;
    }
    void loop() {
        client_set_running_status(true);
        bool running = true;

        while (running) {
            int rc = mosquitto_loop(mosq, 1000, 1);
            if (MOSQ_ERR_NO_CONN == rc || MOSQ_ERR_CONN_LOST == rc) {
                if (_conn_lost)
                    mosquitto_reconnect(mosq);
            }
            running = client_is_running();
        }
    }
    bool client_is_running()
    {
        std::lock_guard<std::mutex> lock(_mutex);
        return _run;
    }
    void client_disconnect()
    {
        mosquitto_disconnect(mosq);
    }
    void client_publish(const char *topic, const void *message, int message_size, int qos, bool retain)
    {
        int mid;
        int rc = mosquitto_publish(mosq, &mid, topic, message_size, message, qos, retain);
        if (MOSQ_ERR_SUCCESS == rc) {
            //printf("Packet ID %d: The message for the topic \"%s\" is being published.\n", mid, topic);
        }
        else {
            printf("Failed to publish the message for \"%s\". Error %d: %s\n", topic, rc, mosquitto_strerror(rc));
        }
    }
    // NOTE: more client methods can from https://github.com/limmh/mqtt_client/blob/master/main.c

    void client_cleanup()
    {
        mosquitto_destroy(mosq);
    }
    void stopAndJoin() {
        client_disconnect();
        client_set_running_status(false);
        // WaitForSingleObject(hThread1, INFINITE);
        hThread1->join();
        delete hThread1;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        client_cleanup();
        mosquitto_lib_cleanup();
    }
    void loopForever() {
        std::thread::id this_id = std::this_thread::get_id();
        g_display_mutex.lock();
        std::cout << "loopForever tid " << this_id << "\n";
        g_display_mutex.unlock();
        while (1) {
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    }

	void testGetMarketInfo();
	void testGetSplitInfo();

private:
	void onResponse(struct mosquitto *m, void *obj, const struct mosquitto_message *msg);

private:
    std::string server;
    int port;
	std::string clientid;
	std::string password;

    struct mosquitto *mosq = NULL;
    std::thread *hThread1 = NULL;
    bool _run = false;
    bool _conn_lost = false;
    std::mutex _mutex;
};

