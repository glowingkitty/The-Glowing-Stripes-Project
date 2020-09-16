class Update():
    def __init__(self):
        self.files = [
            'stripe.py',
            'update.py'
        ]

    def get_code(self, filename):
        f = open(filename, "r")
        filecontent = f.read()
        f.close()
        return filecontent

    def save_code(self, filename, content):
        f = open(filename, "w")
        f.write(content)
        f.close()
        print('Updated file "{}"'.format(filename))

    def get_all_files(self):
        print('Generate response will all files, when they have been updated and their content.')
        response = '['
        for file_name in self.files[:-1]:
            response += """{
                            "file_name":"""+('"'+file_name+'"')+""",
                            "code":"""+('"'+self.get_code(file_name)+'"')+"""
                        },"""

        response += """{
                            "file_name":"""+('"'+self.files[-1]+'"')+""",
                            "code":"""+('"'+self.get_code(self.files[-1])+'"')+"""
                        }]"""

        return response
