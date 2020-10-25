import json
import os
import random
from os import name

dirname = os.path.dirname(__file__)


class LEDanimations():
    def __init__(self):
        self.categories = ['default', 'custom']

    def get_animation(self, id):
        animations = self.all
        for category in self.categories:
            for animation in animations[category]:
                if animation['id'] == id:
                    return animation
        else:
            print('Coudnt find ID')
            return None

    @property
    def last_used(self):
        # return last used LED animation ID
        with open(os.path.join(dirname, 'config.json')) as json_file:
            last_animation = json.load(json_file)['last_animation']
            if not last_animation:
                return None

            else:
                # search for animation in full animation list
                return self.get_animation(last_animation['id'])

    @property
    def all(self):
        # Return all animations from json
        with open(os.path.join(dirname, 'led_animations.json')) as json_file:
            return json.load(json_file)['led_animations']

    def id_exists(self, id):
        # check if id already in json
        with open(os.path.join(dirname, 'led_animations.json')) as json_file:
            animations = json.load(json_file)['led_animations']

        for animation in animations['custom']:
            if id == animation['id']:
                return True
        for animation in animations['default']:
            if id == animation['id']:
                return True

        return False

    def get_random_id(self):
        # generate random ID and make sure it doesn't exist yet in led_animations.json
        new_id = ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                                         'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])
        while self.id_exists(new_id):
            new_id = ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q',
                                             'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])
        return new_id

    def add(self,
            name,
            category='custom',
            based_on=None,
            customization={}
            ):
        # add default or custom animation to json
        with open(os.path.join(dirname, 'led_animations.json')) as json_file:
            animations = json.load(json_file)

        new_animation = {
            "id": self.get_random_id(),
            "name": name,
        }

        # if no customization, make it "default" animation
        if not customization:
            category = 'default'

        if category == 'custom':
            new_animation['based_on'] = based_on
            new_animation['customization'] = customization

        animations['led_animations'][category].append(new_animation)

        with open(os.path.join(dirname, 'led_animations.json'), 'w') as outfile:
            # format json to make it more readable
            json.dump(animations, outfile, indent=4)

        print('Added animation "{}" to {}'.format(name, category))

    def update(self, id, new_name, new_customization={}):
        # update led animation
        with open(os.path.join(dirname, 'led_animations.json')) as json_file:
            animations = json.load(json_file)

        updated = False
        for category in self.categories:
            for animation in animations['led_animations'][category]:
                if animation['id'] == id:
                    animation['name'] = new_name
                    if category == 'custom':
                        animation['customization'] = new_customization

                    with open(os.path.join(dirname, 'led_animations.json'), 'w') as outfile:
                        # format json to make it more readable
                        json.dump(animations, outfile, indent=4)

                    print('Updated animation "{}" in {}'.format(
                        new_name, category))
                    updated = True
                    break
            if updated:
                break
        else:
            print('ERROR: ID {} not found'.format(id))

    def remove(self, id):
        # remove default or custom animation to json
        with open(os.path.join(dirname, 'led_animations.json')) as json_file:
            animations = json.load(json_file)

        removed = False
        for category in self.categories:
            for animation in animations['led_animations'][category]:
                if animation['id'] == id:
                    new_animations = {
                        "led_animations": {
                            "custom": [x for x in animations['led_animations']['custom'] if x != animation],
                            "default": [x for x in animations['led_animations']['default'] if x != animation]
                        }
                    }

                    with open(os.path.join(dirname, 'led_animations.json'), 'w') as outfile:
                        # format json to make it more readable
                        json.dump(new_animations, outfile, indent=4)

                    print('Removed animation "{}" in {}'.format(
                        animation['name'], category))
                    removed = True
                    break

            if removed:
                break
        else:
            print('ERROR: ID {} not found'.format(id))
