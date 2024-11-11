import xml.etree.ElementTree as ET
import os
import logging
import shutil
import xml.dom.minidom
import tkinter as tk
from tkinter import filedialog, messagebox, ttk

# Configure logging
logging.basicConfig(level='INFO', format='%(asctime)s - %(levelname)s - %(message)s')

# Debug mode flag
DEBUG_MODE = True  # Set to True for debug mode, False for normal mode

def find_cfgeconomycore_xml():
    script_dir = os.path.dirname(__file__)
    parent_dir = os.path.dirname(script_dir)
    return os.path.join(parent_dir, 'cfgeconomycore.xml')

def add_ce_elements_to_cfgeconomycore(files):
    cfgeconomycore_file_path = find_cfgeconomycore_xml()
    try:
        tree = ET.parse(cfgeconomycore_file_path)
        root = tree.getroot()
        files = sorted(set(files))
        for ce in root.findall('ce'):
            root.remove(ce)
        for file in files:
            ce_element = ET.Element('ce', folder='types')
            ET.SubElement(ce_element, 'file', name=file, type='types')
            root.append(ce_element)
        tree.write(cfgeconomycore_file_path, encoding='UTF-8', xml_declaration=True)
        logging.info("Added CE elements to cfgeconomycore.xml")
    except (ET.ParseError, FileNotFoundError) as e:
        logging.error(f"Error: {e}")
    except Exception as e:
        logging.error(f"An unexpected error occurred: {e}")

def format_cfgeconomycore():
    cfgeconomycore_file_path = find_cfgeconomycore_xml()
    try:
        tree = ET.parse(cfgeconomycore_file_path)
        root = tree.getroot()
        formatted_xml = '<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>\n<economycore>\n\n'
        formatted_xml += '\t<classes>\n'
        for child in root.find('classes').iter('rootclass'):
            formatted_xml += f'\t\t<{child.tag} name="{child.get("name")}"'
            formatted_xml += ''.join([f' {k}="{v}"' for k, v in child.items() if k != 'name'])
            formatted_xml += '/>\n'
        formatted_xml += '\t</classes>\n\n'
        formatted_xml += '\t<defaults>\n'
        for child in root.find('defaults'):
            formatted_xml += f'\t\t<{child.tag} name="{child.get("name")}" value="{child.get("value")}"/>\n'
        formatted_xml += '\t</defaults>\n\n'
        ce_files = sorted([(ce.find('file').get('name'), ce.find('file').get('type')) for ce in root.findall('ce')])
        if ce_files:
            formatted_xml += '\t<ce folder="types">\n'
            formatted_xml += ''.join([f'\t\t<file name="{name}" type="{type}" />\n' for name, type in ce_files])
            formatted_xml += '\t</ce>\n'
        formatted_xml += '</economycore>\n'
        with open(cfgeconomycore_file_path, 'w') as f:
            f.write(formatted_xml)
        logging.info("Formatted cfgeconomycore.xml")
    except (ET.ParseError, FileNotFoundError) as e:
        logging.error(f"Error: {e}")
    except Exception as e:
        logging.error(f"An unexpected error occurred: {e}")

def save_category_files(categorized_data, types_dir):
    for category, elements in categorized_data.items():
        category_file_path = os.path.join(types_dir, f"{category}.xml")
        root = ET.Element('types')
        for element in elements:
            root.append(element)
        rough_string = ET.tostring(root, encoding='utf-8')
        reparsed = xml.dom.minidom.parseString(rough_string)
        formatted_xml = reparsed.toprettyxml(indent="  ")
        formatted_xml = '\n'.join([line for line in formatted_xml.splitlines() if line.strip()])
        with open(category_file_path, 'w', encoding='utf-8') as f:
            f.write(formatted_xml + '\n')

def categorize_elements(root):
    categories = {
        'ammo': lambda name: name.startswith('Ammo_'),
        'armbands': lambda name: name.startswith('Armband_'),
        'ammo_boxes': lambda name: name.startswith('AmmoBox_'),
        'animals': lambda name: name.startswith('Animal_'),
        'contamination': lambda name: name.startswith(('Land_Container_', 'Land_Train_', 'ContaminatedArea_Dynamic')),
        'flags': lambda name: name.startswith('Flag_'),
        'staticObjs': lambda name: name.startswith('StaticObj_'),
        'vehicles': lambda name: name.startswith(('Offroad', 'CivilianSedan', 'Hatchback', 'Sedan', 'Truck_01', 'Boat_')),
        'wrecks': lambda name: name.startswith(('Land_Wreck_', 'Land_wreck_', 'Wreck_')),
        'zombies': lambda name: name.startswith(('ZmbM_', 'ZmbF_', 'Zmbm_')),
        'seasonal': lambda name: any(usage.get('name') == 'SeasonalEvent' for usage in type_element.findall('usage')) or name.startswith(('ChristmasTree', 'Bonfire', 'EasterEgg', 'Aniversary')),
        'clothes': lambda type_element: (
                            type_element.get('name') in ['GhillieSuit_Tan', 'GhillieAtt_Winter', 'OMKJacket_Navy', 'OMKPants_Navy', 'PlateCarrierHolster_Winter']
                        ) or (
                            type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'clothes'
                        ),
        'explosives': lambda type_element: type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'explosives',
        'containers': lambda type_element: (
                            type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'containers'
                        ) or type_element.get('name') in [
                            'FilteringBottle', 'GlassBottle', 'WaterBottle'
                        ],
        'food': lambda type_element: (
                            type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'food'
                        ) or type_element.get('name') in [
                            'DeadFox', 'ReindeerPelt', 'ReindeerSteakMeat', 'SteelheadTrout', 
                            'SteelheadTroutFilletMeat', 'WalleyePollock', 'WalleyePollockFilletMeat',
                            'CraterellusMushroom', 'RedCaviar'
                        ],
        'tools': lambda type_element: (
                            type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'tools'
                        ) and type_element.get('name') not in ['GhillieSuit_Tan', 'GhillieAtt_Winter', 'OMKJacket_Navy', 'OMKPants_Navy', 'PlateCarrierHolster_Winter'],
        'weapons': lambda type_element: type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'weapons',
        'vehicleParts': lambda type_element: type_element.find('category') is not None and type_element.find('category').get('name', '').lower() == 'lootdispatch',
        'uncategorized': lambda name: True
    }
    categorized_data = {key: [] for key in categories}
    for type_element in root.findall('type'):
        name = type_element.get('name')
        for category, condition in categories.items():
            if category in ['containers', 'vehicleParts', 'weapons', 'tools', 'food', 'clothes', 'explosives'] and condition(type_element):
                categorized_data[category].append(type_element)
                break
            elif category not in ['containers', 'vehicleParts', 'weapons', 'tools', 'food', 'clothes', 'explosives'] and condition(name):
                categorized_data[category].append(type_element)
                break
    return categorized_data

def organize_xml_contents(selected_file_path):
    script_dir = os.path.dirname(selected_file_path)
    original_file_path = selected_file_path
    backup_file_path = os.path.join(script_dir, 'types.bk')
    types_dir = os.path.join(script_dir, 'types')
    if DEBUG_MODE:
        if os.path.exists(types_dir):
            shutil.rmtree(types_dir)
            logging.info("Debug mode: Deleted existing 'types' directory")
    else:
        if os.path.exists(original_file_path):
            os.rename(original_file_path, backup_file_path)
        else:
            logging.error("The specified file 'types.xml' was not found.")
            return
    tree = ET.parse(original_file_path if DEBUG_MODE else backup_file_path)
    root = tree.getroot()
    os.makedirs(types_dir, exist_ok=True)
    categorized_data = categorize_elements(root)
    save_category_files(categorized_data, types_dir)
    add_ce_elements_to_cfgeconomycore([f"{category}.xml" for category in categorized_data if categorized_data[category]])
    format_cfgeconomycore()

class TypesSplitterGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("Types Splitter Pro - v2.126.2")
        self.create_widgets()

    def create_widgets(self):
        self.label = tk.Label(self.root, text="Select the types.xml file to organize:")
        self.label.pack(pady=10)

        self.select_file_button = tk.Button(self.root, text="Browse", command=self.browse_file)
        self.select_file_button.pack(pady=5)

        self.selected_file_label = tk.Label(self.root, text="No file selected", fg="grey")
        self.selected_file_label.pack(pady=5)

        self.debug_mode_var = tk.BooleanVar(value=DEBUG_MODE)
        self.debug_checkbox = tk.Checkbutton(self.root, text="Enable Debug Mode", variable=self.debug_mode_var, command=self.toggle_debug_mode)
        self.debug_checkbox.pack(pady=5)

        self.progress = ttk.Progressbar(self.root, orient="horizontal", length=300, mode="determinate")
        self.progress.pack(pady=10)

        self.start_button = tk.Button(self.root, text="Start Organizing", command=self.start_organizing, state=tk.DISABLED)
        self.start_button.pack(pady=20)

    def toggle_debug_mode(self):
        global DEBUG_MODE
        DEBUG_MODE = self.debug_mode_var.get()
        logging.info(f"Debug mode set to {DEBUG_MODE}")

    def browse_file(self):
        file_path = filedialog.askopenfilename(title="Select types.xml", filetypes=[("XML files", "*.xml")])
        if file_path:
            self.selected_file_path = file_path
            self.selected_file_label.config(text=os.path.basename(file_path), fg="black")
            self.start_button.config(state=tk.NORMAL)
        else:
            self.selected_file_label.config(text="No file selected", fg="grey")
            self.start_button.config(state=tk.DISABLED)

    def start_organizing(self):
        self.progress["value"] = 0
        self.root.update_idletasks()
        try:
            organize_xml_contents(self.selected_file_path)
            self.progress["value"] = 100
            self.root.update_idletasks()
            messagebox.showinfo("Success", "XML contents have been organized successfully!")
        except Exception as e:
            logging.error(f"An unexpected error occurred: {e}")
            messagebox.showerror("Error", f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    root = tk.Tk()
    app = TypesSplitterGUI(root)
    root.mainloop()
