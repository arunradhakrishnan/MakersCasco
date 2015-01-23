import sys
import json
import datetime
from boto import kinesis
from boto.kinesis.exceptions import ResourceNotFoundException

def push_data(conn, 
              stream_name,
              partition_key,
              mac_address,
              event_type,
              value,
              gps_status,
              gps_latitude,
              gps_longitude,
              gps_altitude):
    try:
    
        event = json.dumps({
            'mac_address': mac_address,
            'event_type': event_type,
            'event_time': datetime.datetime.now().isoformat(),
            'value': value,
            'gps_status': gps_status,
            'gps_latitude': gps_latitude,
            'gps_longitude': gps_longitude,
            'gps_altitude': gps_altitude,
        })
        print(event)

        response = conn.put_record(
            stream_name=stream_name,
            data=event,
            partition_key=partition_key)
        print('Status OK. SequenceNumber: {0}'.format(response['SequenceNumber']))

    except Exception as e:
        print(e)
        print('Error: {0}'.format(e))

if __name__ == '__main__':

    if len(sys.argv) == 9:
        try:
            conn = kinesis.connect_to_region('us-west-2',
                aws_access_key_id="AKIAIAQPLP6FOPOWIOWA", 
                aws_secret_access_key="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")

            print('Connected to kinesis...')

            stream_name = "smart_helmet"
            partition_key = sys.argv[1]
            mac_address = sys.argv[2]
            event_type = sys.argv[3]
            value = sys.argv[4]
            gps_status = sys.argv[5]
            gps_latitude = sys.argv[6]
            gps_longitude = sys.argv[7]
            gps_altitude = sys.argv[8]

            print('Pushing data to {0}...'.format(stream_name))
            push_data(conn, 
                      stream_name, 
                      partition_key,
                      mac_address,
                      event_type,
                      value,
                      gps_status,
                      gps_latitude,
                      gps_longitude,
                      gps_altitude)      

        except Exception as e:
            print('Error conecting to Kinesis. {0}'.format(e))
    else:
        print('usage: push_event.py [partition] [mac_address] [event_type] [value] [gps_status] [gps_latitude] [gps_longitude] [gps_altitude]')