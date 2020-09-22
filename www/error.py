import json


def save_error(error):
    print("ERROR:", str(error))
    with open('www/error_log.json') as json_file:
        json_data = json.load(json_file)
        json_data['errors'].append(error[0])

    with open('www/error_log.json', 'w') as outfile:
        json.dump(json_data, outfile)

        print('Saved error')
