import json

# 定义文件路径
LOG_FILE = "../experiment.json"

def classify_types(file_path):
    entity_types = set()  # 存储实体类型
    relation_types = set()  # 存储关系类型
    
    # 打开并逐行读取 JSON 数据
    with open(file_path, 'r') as f:
        for line in f:
            try:
                record = json.loads(line.strip())  # 将每一行解析为 JSON 对象
                if "type" in record:
                    if "name" in record:  # 如果包含 name 字段，属于实体类型
                        entity_types.add(record["type"])
                    elif "subject" in record and "object" in record:  # 如果包含 subject 和 object 字段，属于关系类型
                        relation_types.add(record["type"])
            except json.JSONDecodeError:
                print(f"Error decoding line: {line.strip()}")  # 错误处理

    return entity_types, relation_types

if __name__ == "__main__":
    entity_types, relation_types = classify_types(LOG_FILE)
    
    # 输出实体类型
    print(f"Number of entity types: {len(entity_types)}")
    print("Entity types:")
    for t in sorted(entity_types):
        print(t)
    
    # 输出关系类型
    print(f"\nNumber of relation types: {len(relation_types)}")
    print("Relation types:")
    for t in sorted(relation_types):
        print(t)
