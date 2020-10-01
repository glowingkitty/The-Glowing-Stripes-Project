import json
import random


class StripeDetails():

    @property
    def name(self):
        # read name from config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            return json_data['name']

    def update_name(self, new_name):
        # save name to config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            json_data['name'] = new_name

        with open('config.json', 'w') as outfile:
            json.dump(json_data, outfile)

            print('New name: ', json_data['name'])

    @property
    def id(self):
        # read id from config.json, if it doesn't exist yet, generate one and write it to config
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            if not json_data['id']:
                json_data['id'] = self.get_random_id()
                self.update_id(json_data['id'])
            return json_data['id']

    def get_random_id(self):
        return ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])

    def update_id(self, new_id):
        # save id to config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            json_data['id'] = new_id

        with open('config.json', 'w') as outfile:
            json.dump(json_data, outfile)

            print('New id: ', json_data['id'])
