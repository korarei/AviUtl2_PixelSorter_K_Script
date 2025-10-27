from pathlib import Path

import libs.build_lib as build_lib


def main():
    root = Path(__file__).parent.parent.resolve()
    config = build_lib.load_config(Path(__file__).parent / "build_config.json", root)

    build_lib.build_script(config["paths"]["template"], config["paths"]["script"], config["replacements"])
    build_lib.copy_docs(config["docs"], config["dirs"]["release"])
    build_lib.create_release_note(config["docs"]["README"], config["dirs"]["build"])
    build_lib.create_zip(config["dirs"]["release"], config["dirs"]["build"], config["project"])


if __name__ == "__main__":
    main()
