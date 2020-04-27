# Ansible Config for the Botfarm

## Testing connectivity

```
ansible -i inventory all -m ping
```

## Deploying The Farm

```
ansible-playbook botfarm.yml -i inventory --ask-become-pass
```

## Deploying Local Desktop

```
ansible-playbook desktop.yml --ask-become-pass
```

## Setting up a new host

Copy contents of ~/.ssh/id_rsa.pub into authorized_keys on new host.
